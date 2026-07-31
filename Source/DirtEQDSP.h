#pragma once

#include <cmath>
#include <JuceHeader.h>

class SVFFilter
{
public:
    void reset()
    {
        ic1eq = 0.0;
        ic2eq = 0.0;
    }

    void updateBell(double bellgain_dB, double cutoff, double Q, double srate);
    void updateLowpass(double cutoff, double Q, double srate);
    void updateHighpass(double cutoff, double Q, double srate);

    double process(double v0, double m0, double m1, double m2, double ic1_drive, double ic2_drive);

    // Getters for coefficients to be used in process
    double getM0() const { return m0; }
    double getM1() const { return m1; }
    double getM2() const { return m2; }

private:
    double ic1eq = 0.0;
    double ic2eq = 0.0;
    double g = 0.0, k = 0.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    double m0 = 0.0, m1 = 0.0, m2 = 0.0;
    const double hp = juce::MathConstants<double>::pi * 0.5;
};

class DirtEQDSP
{
public:
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        L1LP.reset(); R1LP.reset();
        L1.reset();   R1.reset();
        L2.reset();   R2.reset();
        L3.reset();   R3.reset();
        L4.reset();   R4.reset();
        L1HP.reset(); R1HP.reset();
    }

    struct Parameters {
        double lowpass_freq = 24000.0;
        double lowpass_q = 0.9;
        
        double band1_gain = 0.0;
        double band1_freq = 10000.0;
        
        double band2_gain = 0.0;
        double band2_freq = 3000.0;
        double band2_q = 0.5;
        
        double band3_gain = 0.0;
        double band3_freq = 500.0;
        double band3_q = 0.5;
        
        double band4_gain = 0.0;
        double band4_freq = 80.0;
        
        double highpass_freq = 5.0;
        double highpass_q = 0.9;
        
        double ic1_drive = 1.0;
        double ic2_drive = 1.0;
        
        double drive_db = 0.0;
        double gain_db = 0.0;
        bool eq_on = true;
    };

    void process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params);
    float getCurrentRMS() const { return currentRMS; }

private:
    double sampleRate = 44100.0;
    float currentRMS = 0.0f;
    
    SVFFilter L1LP, R1LP;
    SVFFilter L1, R1;
    SVFFilter L2, R2;
    SVFFilter L3, R3;
    SVFFilter L4, R4;
    SVFFilter L1HP, R1HP;

    double last_gain = 1.0;
    double last_drive = 1.0;
};
