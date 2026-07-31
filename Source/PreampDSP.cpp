#include "PreampDSP.h"
#include <juce_audio_basics/juce_audio_basics.h>

void PreampDSP::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    rmsCoef = std::exp(-1.0 / (0.050 * sampleRate)); // 50ms RMS window
    inRmsL = 0.0;
    inRmsR = 0.0;
    outRmsL = 0.0;
    outRmsR = 0.0;
}

void PreampDSP::process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params)
{
    if (params.type == 0) return; // Bypass

    // Smooth, musical drive mapping: 0 to +18 dB maximum
    double driveDb = (params.drive / 100.0) * 18.0;
    double driveScale = std::pow(10.0, driveDb / 20.0);
    double trimScale = std::pow(10.0, params.trim / 20.0);

    for (int i = 0; i < numSamples; ++i)
    {
        double spl0 = leftChannel[i];
        double spl1 = rightChannel[i];

        if (params.auto_level) {
            inRmsL = spl0 * spl0 + rmsCoef * (inRmsL - spl0 * spl0);
            inRmsR = spl1 * spl1 + rmsCoef * (inRmsR - spl1 * spl1);
        }

        spl0 *= driveScale;
        spl1 *= driveScale;

        if (params.type == 1) // Telefunken (Vintage Tube)
        {
            auto tubeShaper = [&](double x) {
                // Soft asymmetric tube saturation (rich 2nd harmonic, non-harsh)
                double saturated = std::tanh(x + 0.12 * x * std::abs(x));
                return saturated;
            };
            spl0 = tubeShaper(spl0);
            spl1 = tubeShaper(spl1);
        }
        else if (params.type == 2) // Neve (Class-A Console)
        {
            auto neveShaper = [&](double x) {
                // Class-A transformer saturation with soft knee compression
                return std::tanh(x * 0.95 + 0.05 * x * x * (x > 0 ? 1.0 : -1.0));
            };
            spl0 = neveShaper(spl0);
            spl1 = neveShaper(spl1);
        }
        else if (params.type == 3) // Modern (Tape & Saturation)
        {
            auto tapeShaper = [](double x) {
                // Smooth triode/tape soft saturation curve
                double ax = std::abs(x);
                if (ax < 0.7) return x;
                return (x > 0 ? 1.0 : -1.0) * (0.7 + 0.3 * std::tanh((ax - 0.7) / 0.3));
            };
            spl0 = tapeShaper(spl0);
            spl1 = tapeShaper(spl1);
        }

        if (params.auto_level) {
            outRmsL = spl0 * spl0 + rmsCoef * (outRmsL - spl0 * spl0);
            outRmsR = spl1 * spl1 + rmsCoef * (outRmsR - spl1 * spl1);
            
            if (++counter >= 32) {
                double inRms = std::sqrt((inRmsL + inRmsR) * 0.5);
                double outRms = std::sqrt((outRmsL + outRmsR) * 0.5);
                cachedMakeup = (outRms > 0.00001) ? (inRms / outRms) : 1.0;
                cachedMakeup = std::max(0.1, std::min(cachedMakeup, 10.0));
                counter = 0;
            }
            
            spl0 *= cachedMakeup;
            spl1 *= cachedMakeup;
        } else {
            spl0 /= driveScale;
            spl1 /= driveScale;
        }

        // Trim scales the drive level into the next DSP stage (Compressor)
        spl0 *= trimScale;
        spl1 *= trimScale;

        leftChannel[i] = static_cast<float>(spl0);
        rightChannel[i] = static_cast<float>(spl1);
        
        double maxAbs = std::max(std::abs(spl0), std::abs(spl1));
        currentRMS = currentRMS * 0.999f + static_cast<float>(maxAbs) * 0.001f;
    }
}
