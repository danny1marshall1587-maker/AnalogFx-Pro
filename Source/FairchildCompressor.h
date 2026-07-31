#pragma once

#include <cmath>
#include <algorithm>

class FairchildCompressor
{
public:
    void prepare(double newSampleRate);

    struct Parameters {
        double l_thresh = 0.0;
        double r_thresh = 0.0;
        double l_bias = 70.0;
        double r_bias = 70.0;
        double l_makeup = 0.0;
        double r_makeup = 0.0;
        int agc = 0; // 0 = L/R, 1 = Lat/Vert
        int l_time = 1;
        int r_time = 1;
        double l_rms = 100.0;
        double r_rms = 100.0;
    };

    void process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params);
    float getCurrentRMS() const { return currentRMS; }
    float getGainReduction() const { return currentGR; }

private:
    double sampleRate = 44100.0;
    float currentRMS = 0.0f;
    float currentGR = 0.0f;

    double lrunave = 0.0;
    double lrundb = 0.0;
    double rrunave = 0.0;
    double rrundb = 0.0;

    double db2log = 0.11512925464970228420089957273422;
    double log2db = 8.6858896380650365530225783783321;
    double dcoffset = 0.0001;
};
