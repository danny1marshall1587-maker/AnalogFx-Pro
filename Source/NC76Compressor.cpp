#include "NC76Compressor.h"
#include <cstdint>
#include <algorithm>

void NC76Compressor::process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params)
{
    const double log2db = 8.68588963806503655302;
    const double db2log = 0.11512925464970228420;

    double ratio = 4.0;
    double cratio = 4.0;
    bool allin = (params.ratio_idx == 99);

    if (params.ratio_idx == 4) ratio = 4.0;
    else if (params.ratio_idx == 8) ratio = 8.0;
    else if (params.ratio_idx == 12) ratio = 12.0;
    else if (params.ratio_idx == 20) ratio = 20.0;
    
    cratio = allin ? 12.0 : ratio;

    double cthreshv = std::exp(params.threshold * db2log);
    double makeupv = std::exp(params.out_gain_db * db2log);
    
    double attime = 2.0 * params.attack_us / 1000000.0;
    if (params.punch) attime += 0.008;

    double reltime = params.release_ms / 1000.0;
    if (allin) reltime /= 10.0;
    if (params.fast_release) reltime /= 5.0;

    double atcoef = std::exp(-1.0 / (attime * sampleRate));
    double relcoef = std::exp(-1.0 / (reltime * sampleRate));

    double mix = params.mix / 100.0;
    double ingain = 3.98107170553 * params.in_gain; // exp(db2log * 12) is ~3.98

    double ovrlthresh = 0.12589254117; // 10^((2*1.5)-2) is 0.125...
    double ovrlatt = 0.07943282347; // 10^(-0.01 - 1)
    double ovrlrel = 0.00316227766; // 10^(-2.5)

    for (int i = 0; i < numSamples; ++i)
    {
        double spl0 = leftChannel[i] * ingain;
        double spl1 = rightChannel[i] * ingain;

        double ospl0 = spl0;
        double ospl1 = spl1;

        double det = std::sqrt(std::max(1e-12, (spl0 * spl0 + spl1 * spl1) * 0.5));
        runave = det + atcoef * (runave - det);
        
        double overdb = 0.0;
        if (runave > cthreshv) {
            overdb = log2db * std::log(runave / cthreshv);
        }

        if (overdb > rundb) {
            rundb = overdb + atcoef * (rundb - overdb);
        } else {
            rundb = overdb + relcoef * (rundb - overdb);
        }

        double gr = -rundb * (allin ? 1.4 : 1.0) * (cratio - 1.0) / cratio;
        double grv = std::exp(gr * db2log);

        currentGR = static_cast<float>(gr); 

        spl0 *= grv * makeupv;
        spl1 *= grv * makeupv;

        if (mix < 1.0) {
            spl0 = spl0 * mix + ospl0 * (1.0 - mix);
            spl1 = spl1 * mix + ospl1 * (1.0 - mix);
        }

        // Limiter
        double ovrllev = (0.5 * ovrlg * std::abs(spl0 + spl1));
        if (ovrllev > ovrlthresh) ovrlg -= ovrlatt * (ovrllev - ovrlthresh);
        else ovrlg += ovrlrel * (1.0 - ovrlg);

        spl0 *= ovrlg;
        spl1 *= ovrlg;

        leftChannel[i] = static_cast<float>(spl0);
        rightChannel[i] = static_cast<float>(spl1);
        
        double maxAbs = std::max(std::abs(spl0), std::abs(spl1));
        currentRMS = currentRMS * 0.999f + static_cast<float>(maxAbs) * 0.001f;
    }
}
