#pragma once

#include <cmath>
#include <algorithm>

class NC76Compressor
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        ratatcoef = std::exp(-1.0 / (0.00001 * sampleRate));
        ratrelcoef = std::exp(-1.0 / (0.5 * sampleRate));
        
        rundb = 0.0;
        runratio = 4.0; // default ratio
        averatio = 4.0;
        
        runave = 0.0;
        ovrlg = 1.0;
    }

    struct Parameters {
        double threshold = 0.0; // dB
        int ratio_idx = 4; // 4, 8, 12, 20, 99 (All in)
        double out_gain_db = -10.0; // dB
        double attack_us = 100.0;
        double release_ms = 250.0;
        double mix = 100.0; // %
        double in_gain = 0.77; // raw multiplier
        bool punch = false;
        bool fast_release = false;
    };

    void process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params);
    float getCurrentRMS() const { return currentRMS; }
    float getGainReduction() const { return currentGR; }

private:
    double sampleRate = 44100.0;
    float currentRMS = 0.0f;
    float currentGR = 0.0f;

    double ratatcoef = 0.0;
    double ratrelcoef = 0.0;

    double rundb = 0.0;
    double runratio = 0.0;
    double averatio = 0.0;
    double runave = 0.0;
    double ovrlg = 1.0;
};
