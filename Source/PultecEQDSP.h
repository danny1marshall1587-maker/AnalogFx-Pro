#pragma once

#include <cmath>
#include <algorithm>

class PultecEQDSP
{
public:
    void prepare(double newSampleRate);

    struct Parameters {
        int lsf_freq = 0; // 0=20, 1=30, 2=60, 3=100
        double low_boost = 0.0; // 0 to 10
        double low_atten = 0.0; // 0 to 10
        int peak_freq = 4; // 0=3k, 1=4k, 2=5k, 3=8k, 4=10k, 5=12k, 6=16k
        double peak_q = 2.77; // 0 to 10
        double peak_boost = 0.0; // 0 to 10
        int hsf_freq = 2; // 0=5k, 1=10k, 2=20k
        double high_atten = 0.0; // 0 to 10
        double trim = 0.0;
    };

    void process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params);
    float getCurrentRMS() const { return currentRMS; }

private:
    double sampleRate = 44100.0;
    float currentRMS = 0.0f;
    
    // LowPass A (Boost)
    double x1AA=0, x2AA=0, y1AA=0, y2AA=0;
    double x1BA=0, x2BA=0, y1BA=0, y2BA=0;
    
    // LowPass B (Atten)
    double x1AB=0, x2AB=0, y1AB=0, y2AB=0;
    double x1BB=0, x2BB=0, y1BB=0, y2BB=0;
    
    // Peak (Boost)
    double x1AC=0, x2AC=0, y1AC=0, y2AC=0;
    double x1BC=0, x2BC=0, y1BC=0, y2BC=0;
    
    // HighShelf (Atten)
    double x1AD=0, x2AD=0, y1AD=0, y2AD=0;
    double x1BD=0, x2BD=0, y1BD=0, y2BD=0;
    
    double twetA = 0.0, twetB = 0.0;
};
