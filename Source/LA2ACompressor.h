#pragma once

#include <cmath>
#include <algorithm>

class BiquadFilter
{
public:
    void setCoefficients(double a0, double a1, double a2, double b0, double b1, double b2)
    {
        this->a1 = a1 / a0;
        this->a2 = a2 / a0;
        this->b0 = b0 / a0;
        this->b1 = b1 / a0;
        this->b2 = b2 / a0;
    }

    double process(double in)
    {
        double y = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        x2 = x1;
        x1 = in;
        y2 = y1;
        y1 = y;
        return y;
    }

    void reset()
    {
        x1 = x2 = y1 = y2 = 0.0;
    }

private:
    double a1=0, a2=0, b0=1, b1=0, b2=0;
    double x1=0, x2=0, y1=0, y2=0;
};

class LA2ACompressor
{
public:
    void prepare(double newSampleRate);

    struct Parameters {
        double peak_reduction = 33.333; // 0-100
        int ratio_limit = 0; // 0=Compress, 1=Limit
        double gain = 33.333; // 0-100
    };

    void process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params);
    float getCurrentRMS() const { return currentRMS; }
    float getGainReduction() const { return currentGR; }

private:
    double sampleRate = 44100.0;
    float currentRMS = 0.0f;
    float currentGR = 0.0f;
    double attimes[120];

    double runave = 0.0;
    double rundb = 0.0;
    double maxover = 0.0;
    double runmax = 0.0;
    double runospl = 0.0;

    double ratio2db(double ratio) { return 20.0 * std::log10(ratio); }
    double db2log = 0.11512925464970228420089957273422;

    BiquadFilter HSL, HSR;
    BiquadFilter LSL, LSR;
};
