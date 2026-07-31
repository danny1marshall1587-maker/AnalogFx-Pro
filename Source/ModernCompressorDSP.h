#pragma once
#include <cmath>
#include <algorithm>

class ModernCompressorDSP {
public:
    void prepare(double newSampleRate) {
        sampleRate = newSampleRate;
        env = 0.0;
    }
    
    struct Parameters {
        double threshold = 0.0;
        double ratio = 4.0;
        double attack = 1.0; // ms
        double release = 100.0; // ms
        double makeup = 0.0;
        double trim = 0.0;
    };
    
    void process(float* left, float* right, int numSamples, const Parameters& params) {
        double attCoef = std::exp(-1.0 / (params.attack * 0.001 * sampleRate));
        double relCoef = std::exp(-1.0 / (params.release * 0.001 * sampleRate));
        double makeupScale = std::pow(10.0, params.makeup / 20.0);
        double trimScale = std::pow(10.0, params.trim / 20.0);
        
        for (int i = 0; i < numSamples; ++i) {
            double spl0 = left[i];
            double spl1 = right[i];
            
            double det = std::max(std::abs(spl0), std::abs(spl1));
            double detDb = 20.0 * std::log10(std::max(det, 1e-6));
            
            double overDb = std::max(0.0, detDb - params.threshold);
            
            if (overDb > env) env = overDb + attCoef * (env - overDb);
            else env = overDb + relCoef * (env - overDb);
            
            double grDb = -env * (params.ratio - 1.0) / params.ratio;
            double grLinear = std::pow(10.0, grDb / 20.0);
            
            currentGR = static_cast<float>(grDb);

            spl0 *= grLinear * makeupScale * trimScale;
            spl1 *= grLinear * makeupScale * trimScale;
            
            left[i] = static_cast<float>(spl0);
            right[i] = static_cast<float>(spl1);
            
            double maxAbs = std::max(std::abs(spl0), std::abs(spl1));
            currentRMS = currentRMS * 0.999f + static_cast<float>(maxAbs) * 0.001f;
        }
    }
    
    float getCurrentRMS() const { return currentRMS; }
    float getGainReduction() const { return currentGR; }
    
private:
    double sampleRate = 44100.0;
    float currentRMS = 0.0f;
    float currentGR = 0.0f;
    double env = 0.0;
};
