#pragma once
#include "DSPFilters.h"

class ModernEQDSP {
public:
    void prepare(double newSampleRate) {
        sampleRate = newSampleRate;
    }
    
    struct Parameters {
        double lowFreq = 100.0, lowGain = 0.0;
        double mid1Freq = 1000.0, mid1Gain = 0.0, mid1Q = 1.0;
        double mid2Freq = 3000.0, mid2Gain = 0.0, mid2Q = 1.0;
        double highFreq = 10000.0, highGain = 0.0;
        double trim = 0.0;
    };
    
    void process(float* left, float* right, int numSamples, const Parameters& params) {
        f_L1.setParams(params.lowFreq, 0.707, params.lowGain, 1, sampleRate);
        f_L2.setParams(params.lowFreq, 0.707, params.lowGain, 1, sampleRate);
        
        f_M1_1.setParams(params.mid1Freq, params.mid1Q, params.mid1Gain, 0, sampleRate);
        f_M1_2.setParams(params.mid1Freq, params.mid1Q, params.mid1Gain, 0, sampleRate);
        
        f_M2_1.setParams(params.mid2Freq, params.mid2Q, params.mid2Gain, 0, sampleRate);
        f_M2_2.setParams(params.mid2Freq, params.mid2Q, params.mid2Gain, 0, sampleRate);
        
        f_H1.setParams(params.highFreq, 0.707, params.highGain, 2, sampleRate);
        f_H2.setParams(params.highFreq, 0.707, params.highGain, 2, sampleRate);
        
        double trimScale = std::pow(10.0, params.trim / 20.0);
        
        for (int i = 0; i < numSamples; ++i) {
            double spl0 = left[i];
            double spl1 = right[i];
            
            spl0 = f_L1.process(spl0);
            spl1 = f_L2.process(spl1);
            
            spl0 = f_M1_1.process(spl0);
            spl1 = f_M1_2.process(spl1);
            
            spl0 = f_M2_1.process(spl0);
            spl1 = f_M2_2.process(spl1);
            
            spl0 = f_H1.process(spl0);
            spl1 = f_H2.process(spl1);
            
            spl0 *= trimScale;
            spl1 *= trimScale;
            
            left[i] = static_cast<float>(spl0);
            right[i] = static_cast<float>(spl1);
            
            double maxAbs = std::max(std::abs(spl0), std::abs(spl1));
            currentRMS = currentRMS * 0.99f + static_cast<float>(maxAbs) * 0.01f;
        }
    }
    
    float getCurrentRMS() const { return currentRMS; }
    
private:
    double sampleRate = 44100.0;
    float currentRMS = 0.0f;
    CustomDSP::EQFilter f_L1, f_L2, f_M1_1, f_M1_2, f_M2_1, f_M2_2, f_H1, f_H2;
};
