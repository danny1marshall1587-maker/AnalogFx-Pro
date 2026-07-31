#include "DirtEQDSP.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <algorithm>

void SVFFilter::updateBell(double bellgain_dB, double cutoff, double Q, double srate)
{
    double a = std::pow(10.0, bellgain_dB / 40.0);
    g = std::tan(juce::MathConstants<double>::pi * cutoff / srate);
    k = 1.0 / (Q * a);
    a1 = 1.0 / (1.0 + g * (g + k));
    a2 = g * a1;
    a3 = g * a2;
    m0 = 1.0;
    m1 = k * (a * a - 1.0);
    m2 = 0.0;
}

void SVFFilter::updateLowpass(double cutoff, double Q, double srate)
{
    g = std::tan(0.5 * juce::MathConstants<double>::pi * cutoff / srate); // Oversampling factor accounted in g for LP
    k = 1.0 / Q;
    a1 = 1.0 / (1.0 + g * (g + k));
    a2 = g * a1;
    a3 = g * a2;
    m0 = 0.0;
    m1 = 0.0;
    m2 = 1.0;
}

void SVFFilter::updateHighpass(double cutoff, double Q, double srate)
{
    g = std::tan(juce::MathConstants<double>::pi * cutoff / srate);
    k = 1.0 / Q;
    a1 = 1.0 / (1.0 + g * (g + k));
    a2 = g * a1;
    a3 = g * a2;
    m0 = 1.0;
    m1 = -k;
    m2 = -1.0;
}

double SVFFilter::process(double v0, double _m0, double _m1, double _m2, double ic1_drive, double ic2_drive)
{
    double v3 = v0 - ic2eq;
    double v1 = a1 * ic1eq + a2 * v3;
    double v2 = ic2eq + a2 * ic1eq + a3 * v3;

    auto fastSin = [](double x) {
        // Fast sin approximation for saturation
        double x2 = x * x;
        return x * (1.0 - x2 * (1.0 / 6.0 - x2 * (1.0 / 120.0)));
    };

    ic1eq = 2.0 * v1 - ic1eq;
    ic1eq *= ic1_drive;
    if (ic1eq > hp) ic1eq = hp; else if (ic1eq < -hp) ic1eq = -hp;
    ic1eq = fastSin(ic1eq);
    ic1eq /= ic1_drive;

    ic2eq = 2.0 * v2 - ic2eq;
    double ic2_d = ic2_drive * 0.41;
    ic2eq *= ic2_d;
    if (ic2eq > hp) ic2eq = hp; else if (ic2eq < -hp) ic2eq = -hp;
    ic2eq = fastSin(ic2eq);
    ic2eq /= ic2_d;

    return _m0 * v0 + _m1 * v1 + _m2 * v2;
}

void DirtEQDSP::process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params)
{
    // Update coefficients once per buffer (much more efficient!)
    L1LP.updateLowpass(params.lowpass_freq, params.lowpass_q, sampleRate);
    R1LP.updateLowpass(params.lowpass_freq, params.lowpass_q, sampleRate);
    
    L1.updateBell(params.band1_gain, params.band1_freq, 0.2, sampleRate);
    R1.updateBell(params.band1_gain, params.band1_freq, 0.2, sampleRate);
    
    L2.updateBell(params.band2_gain, params.band2_freq, params.band2_q, sampleRate);
    R2.updateBell(params.band2_gain, params.band2_freq, params.band2_q, sampleRate);
    
    L3.updateBell(params.band3_gain, params.band3_freq, params.band3_q, sampleRate);
    R3.updateBell(params.band3_gain, params.band3_freq, params.band3_q, sampleRate);
    
    L4.updateBell(params.band4_gain, params.band4_freq, 0.3, sampleRate);
    R4.updateBell(params.band4_gain, params.band4_freq, 0.3, sampleRate);
    
    L1HP.updateHighpass(params.highpass_freq, params.highpass_q, sampleRate);
    R1HP.updateHighpass(params.highpass_freq, params.highpass_q, sampleRate);

    double next_gain = std::pow(10.0, params.gain_db / 20.0);
    double next_drive = std::pow(10.0, params.drive_db / 20.0);
    
    double d_gain = (next_gain - last_gain) / numSamples;
    double d_drive = (next_drive - last_drive) / numSamples;

    for (int i = 0; i < numSamples; ++i)
    {
        double spl0 = leftChannel[i] * last_drive;
        double spl1 = rightChannel[i] * last_drive;

        if (params.eq_on) {
            spl0 = L1LP.process(spl0, L1LP.getM0(), L1LP.getM1(), L1LP.getM2(), params.ic1_drive, params.ic2_drive);
            spl1 = R1LP.process(spl1, R1LP.getM0(), R1LP.getM1(), R1LP.getM2(), params.ic1_drive, params.ic2_drive);

            spl0 = L1.process(spl0, L1.getM0(), L1.getM1(), L1.getM2(), params.ic1_drive, params.ic2_drive);
            spl1 = R1.process(spl1, R1.getM0(), R1.getM1(), R1.getM2(), params.ic1_drive, params.ic2_drive);
            
            spl0 = L2.process(spl0, L2.getM0(), L2.getM1(), L2.getM2(), params.ic1_drive, params.ic2_drive);
            spl1 = R2.process(spl1, R2.getM0(), R2.getM1(), R2.getM2(), params.ic1_drive, params.ic2_drive);
            
            spl0 = L3.process(spl0, L3.getM0(), L3.getM1(), L3.getM2(), params.ic1_drive, params.ic2_drive);
            spl1 = R3.process(spl1, R3.getM0(), R3.getM1(), R3.getM2(), params.ic1_drive, params.ic2_drive);
            
            spl0 = L4.process(spl0, L4.getM0(), L4.getM1(), L4.getM2(), params.ic1_drive, params.ic2_drive);
            spl1 = R4.process(spl1, R4.getM0(), R4.getM1(), R4.getM2(), params.ic1_drive, params.ic2_drive);

            spl0 = spl0 + 0.015625 * spl0 * spl0;
            spl1 = spl1 - 0.015625 * spl1 * spl1;

            spl0 = L1HP.process(spl0, L1HP.getM0(), L1HP.getM1(), L1HP.getM2(), params.ic1_drive, params.ic2_drive);
            spl1 = R1HP.process(spl1, R1HP.getM0(), R1HP.getM1(), R1HP.getM2(), params.ic1_drive, params.ic2_drive);
        }

        spl0 /= last_drive;
        spl1 /= last_drive;
        last_drive += d_drive;
        
        spl0 *= last_gain;
        spl1 *= last_gain;
        last_gain += d_gain;

        leftChannel[i] = static_cast<float>(spl0);
        rightChannel[i] = static_cast<float>(spl1);
    }
    
    double maxAbs = std::max(std::abs(leftChannel[0]), std::abs(rightChannel[0]));
    currentRMS = currentRMS * 0.9f + static_cast<float>(maxAbs) * 0.1f;
}
