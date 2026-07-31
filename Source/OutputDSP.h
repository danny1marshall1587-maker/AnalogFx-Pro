#pragma once

#include <cmath>
#include <algorithm>

class OutputDSP
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
    }

    struct Parameters {
        int type = 0; // 0: Bypass, 1: Tape, 2: Console, 3: Valve, 4: Modern
        double drive = 0.0; // 0-100%
        bool safe_level = false;
    };

    void process(float* left, float* right, int numSamples, const Parameters& params)
    {
        double driveDb = (params.drive / 100.0) * 12.0; 
        double driveScale = std::pow(10.0, driveDb / 20.0);
        
        // Industry standard -18 dBFS for analog-modeled headroom
        const double targetLevel = 0.1258925411794167; 
        
        for (int i = 0; i < numSamples; ++i)
        {
            double spl0 = left[i];
            double spl1 = right[i];

            if (params.type > 0)
            {
                spl0 *= driveScale;
                spl1 *= driveScale;

                if (params.type == 1) // Vintage Tape
                {
                    auto tapeShaper = [](double x) {
                        double ax = std::abs(x);
                        if (ax < 0.6) return x;
                        return (x > 0 ? 1.0 : -1.0) * (0.6 + 0.35 * std::tanh((ax - 0.6) / 0.35));
                    };
                    spl0 = tapeShaper(spl0);
                    spl1 = tapeShaper(spl1);
                }
                else if (params.type == 2) // British Iron
                {
                    auto transShaper = [](double x) {
                        return std::tanh(x * 0.9 + 0.08 * x * std::abs(x));
                    };
                    spl0 = transShaper(spl0);
                    spl1 = transShaper(spl1);
                }
                else if (params.type == 3) // Valve Summing
                {
                    auto valveShaper = [](double x) {
                        double sat = std::tanh(x + 0.1 * x * std::abs(x));
                        return sat / 1.05;
                    };
                    spl0 = valveShaper(spl0);
                    spl1 = valveShaper(spl1);
                }
                else if (params.type == 4) // Modern Polish
                {
                    auto polishShaper = [](double x) {
                        double ax = std::abs(x);
                        if (ax < 0.75) return x;
                        return (x > 0 ? 1.0 : -1.0) * (0.75 + 0.22 * std::tanh((ax - 0.75) / 0.22));
                    };
                    spl0 = polishShaper(spl0);
                    spl1 = polishShaper(spl1);
                }

                spl0 /= driveScale;
                spl1 /= driveScale;
            }

            if (params.safe_level)
            {
                // RMS Detection for the AGC
                double curAbs = std::max(std::abs(spl0), std::abs(spl1));
                rmsLevel = rmsLevel * 0.9999 + curAbs * 0.0001; // Slower, more musical detection
                
                if (rmsLevel > 0.0001) {
                    double targetGain = targetLevel / (rmsLevel + 1e-6);
                    targetGain = std::clamp(targetGain, 0.05, 4.0);
                    
                    // Smooth gain adjustment to prevent zipper noise
                    autoGain += (targetGain - autoGain) * 0.0001; 
                    
                    spl0 *= autoGain;
                    spl1 *= autoGain;
                }
                
                // Final "Safety Ceiling" at -0.1 dB
                auto ceiling = [](double x) {
                    double ax = std::abs(x);
                    if (ax < 0.95) return x;
                    return (x > 0 ? 1 : -1) * (0.95 + 0.04 * std::tanh((ax - 0.95) / 0.04));
                };
                spl0 = ceiling(spl0);
                spl1 = ceiling(spl1);
            }

            left[i] = static_cast<float>(spl0);
            right[i] = static_cast<float>(spl1);
        }
    }

private:
    double sampleRate = 44100.0;
    double rmsLevel = 0.01;
    double autoGain = 1.0;
};
