#pragma once

#include <cmath>
#include <algorithm>

class PreampDSP
{
public:
    void prepare(double newSampleRate);

    struct Parameters {
        int type = 0; // 0=Bypass, 1=Telefunken, 2=Neve, 3=Modern
        double drive = 0.0; // 0 to 100%
        double trim = 0.0; // dB
        bool auto_level = true;
    };

    void process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params);
    
    float getCurrentRMS() const { return currentRMS; }

private:
    double sampleRate = 44100.0;
    
    double inRmsL = 0.0;
    double inRmsR = 0.0;
    double outRmsL = 0.0;
    double outRmsR = 0.0;
    
    double rmsCoef = 0.0;
    int counter = 0;
    double cachedMakeup = 1.0;
    
    float currentRMS = 0.0f;
};
