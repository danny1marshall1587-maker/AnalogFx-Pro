#include "LA2ACompressor.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <cstdint>

void LA2ACompressor::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    for (int i = 0; i < 120; ++i)
    {
        if (i == 0) attimes[i] = 0.01;
        else attimes[i] = 2.0 * (((0.08924 / i) + (0.60755 / (i * i)) - 0.00006));
    }

    runave = 0.0;
    rundb = 0.0;
    maxover = 0.0;
    runmax = 0.0;
    runospl = 0.0;

    // Hardcode the HS and LS filters from the script
    HSL.reset(); HSR.reset();
    LSL.reset(); LSR.reset();

    // Calculate High Shelf (HSSLDR)
    double HS_tmpx = 16.0 + 85.24 * 1.20103;
    double HS_tmpy = std::floor(std::exp(HS_tmpx * std::log(1.059)) * 8.17742);
    double HS_omega = 2.0 * juce::MathConstants<double>::pi * HS_tmpy / sampleRate;
    double HS_K = std::tan(HS_omega / 2.0);
    double HS_Q = std::max(std::min(0.42, 4.0), 0.2);
    double HS_V = std::pow(10.0, -2.0 / 20.0);
    
    double HS_b0 = 1.0 + HS_K/HS_Q + HS_K*HS_K;
    double HS_b1 = -2.0*(HS_K*HS_K - 1.0);
    double HS_b2 = 1.0 - HS_K/HS_Q + HS_K*HS_K;
    double HS_a0 = 1.0 + std::sqrt(HS_V)*HS_K/HS_Q + HS_V*HS_K*HS_K;
    double HS_a1 = -2.0*(HS_V*HS_K*HS_K - 1.0);
    double HS_a2 = 1.0 - std::sqrt(HS_V)*HS_K/HS_Q + HS_V*HS_K*HS_K;
    
    HSL.setCoefficients(HS_a0, HS_a1, HS_a2, HS_b0, HS_b1, HS_b2);
    HSR.setCoefficients(HS_a0, HS_a1, HS_a2, HS_b0, HS_b1, HS_b2);

    // Calculate Low Shelf (LSSLDR)
    double LS_tmpx = 16.0 + 35.78 * 1.20103;
    double LS_tmpy = std::floor(std::exp(LS_tmpx * std::log(1.059)) * 8.17742);
    double LS_omega = 2.0 * juce::MathConstants<double>::pi * LS_tmpy / sampleRate;
    double LS_K = std::tan(LS_omega / 2.0);
    double LS_Q = std::max(std::min(0.282, 4.0), 0.2);
    double LS_V = std::pow(10.0, 0.0 / 20.0); // 1.0
    
    double LS_b0 = 1.0 + LS_K/LS_Q + LS_K*LS_K;
    double LS_b1 = 2.0*(LS_K*LS_K - 1.0);
    double LS_b2 = 1.0 - LS_K/LS_Q + LS_K*LS_K;
    double LS_a0 = 1.0 + std::sqrt(LS_V)*LS_K/LS_Q + LS_V*LS_K*LS_K;
    double LS_a1 = 2.0*(LS_V*LS_K*LS_K - 1.0);
    double LS_a2 = 1.0 - std::sqrt(LS_V)*LS_K/LS_Q + LS_V*LS_K*LS_K;
    
    LSL.setCoefficients(LS_a0, LS_a1, LS_a2, LS_b0, LS_b1, LS_b2);
    LSR.setCoefficients(LS_a0, LS_a1, LS_a2, LS_b0, LS_b1, LS_b2);
}

void LA2ACompressor::process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params)
{
    const double db2log = 0.11512925464970228420;
    
    double vslider1 = (params.peak_reduction / 100.0) * 60.0;
    double vslider3 = (params.gain / 100.0) * 30.0 - 10.0;
    
    double thresh = -vslider1 / 1.2;
    double ratio = (params.ratio_limit == 0) ? 4.0 : 8.0;
    double makeupv = std::exp(vslider3 * db2log);
    double rmscoef = std::exp(-1000.0 / (0.0025 * sampleRate));

    for (int i = 0; i < numSamples; ++i)
    {
        double spl0 = leftChannel[i] * 10.0;
        double spl1 = rightChannel[i] * 10.0;

        double maxspl = std::max(std::abs(spl0), std::abs(spl1));
        runave = maxspl * maxspl + rmscoef * (runave - maxspl * maxspl);
        double det = std::sqrt(std::max(1e-12, runave));

        double mydbin = 8.685889638 * std::log(det); // log2db * log(det)
        
        double knee_width = std::abs(thresh * 0.2);
        double Thi = thresh + knee_width * 0.5;
        double Tlo = thresh - knee_width * 0.5;
        double slope = (1.0 - ratio) / ratio;
        
        double overdb = 0.0;
        if (mydbin >= Thi) {
            overdb = std::abs(slope) * (mydbin - thresh);
        } else if (mydbin > Tlo) {
            double mydelta = mydbin - Tlo;
            overdb = mydelta * mydelta * (std::abs(slope) / (knee_width * 2.0));
        }

        if (overdb > maxover) {
            maxover = overdb;
            int idx = std::min(119, (int)std::floor(std::abs(overdb)));
            double atcoef = std::exp(-1.0 / (attimes[idx] * sampleRate));
            rundb = overdb + atcoef * (rundb - overdb);
        } else {
            double reltime = std::max(0.001, overdb / 25.0);
            double relcoef = std::exp(-1.0 / (reltime * sampleRate));
            rundb = overdb + relcoef * (rundb - overdb);
        }

        double gr_db = -std::max(0.0, rundb) * (ratio - 1.0) / ratio;
        double grv = std::exp(gr_db * db2log);
        currentGR = static_cast<float>(gr_db);

        spl0 = (spl0 / 10.0) * grv * makeupv;
        spl1 = (spl1 / 10.0) * grv * makeupv;
        
        leftChannel[i] = static_cast<float>(spl0);
        rightChannel[i] = static_cast<float>(spl1);

        runmax = maxover + std::exp(-1.0 / (std::max(0.001, maxover/25.0)*sampleRate)) * (runmax - maxover);
        maxover = runmax;

        spl0 = leftChannel[i] * grv * makeupv;
        spl1 = rightChannel[i] * grv * makeupv;

        spl0 = HSL.process(spl0);
        spl1 = HSR.process(spl1);
        spl0 = LSL.process(spl0);
        spl1 = LSR.process(spl1);

        // Hardware-style soft clipper/saturation
        auto softClip = [](double x) {
            double absX = std::abs(x);
            if (absX > 1.0) return (x > 0) ? 1.0 : -1.0;
            return x * (1.5 - 0.5 * absX * absX);
        };
        
        spl0 = softClip(spl0);
        spl1 = softClip(spl1);

        leftChannel[i] = static_cast<float>(spl0);
        rightChannel[i] = static_cast<float>(spl1);
        
        double maxAbs = std::max(std::abs(spl0), std::abs(spl1));
        currentRMS = currentRMS * 0.999f + static_cast<float>(maxAbs) * 0.001f;
    }
}
