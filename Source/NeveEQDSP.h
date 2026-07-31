#pragma once

#include "DSPFilters.h"

class NeveEQDSP
{
public:
    void prepare(double newSampleRate);

    struct Parameters {
        double gain = 0.0;
        double hf_gain = 0.0;
        double mf_gain = 0.0;
        int mf_freq = 0; // 0=Off, 1=0.36k, 2=0.7k, 3=1.6k, 4=3.2k, 5=4.8k, 6=7.2k
        double lf_gain = 0.0;
        int lf_freq = 0; // 0=Off, 1=35, 2=60, 3=110, 4=220
        int hpf_freq = 0; // 0=Off, 1=50, 2=80, 3=160, 4=300
        double drive = 0.0; // 0-100% saturation
        bool phase = false;
        bool hf_on = true;
        bool mf_on = true;
        bool lf_on = true;
        bool hpf_on = true;
        bool eq_on = true;
        double trim = 0.0;
    };

    void process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params);
    float getCurrentRMS() const { return currentRMS; }

private:
    double sampleRate = 44100.0;
    float currentRMS = 0.0f;

    CustomDSP::HIPFilter f_in_1, f_in_2, f_in_3, f_in_4;
    CustomDSP::BUTHPFilter f_SLF_1, f_SLF_2, f_SLF_3, f_SLF_4;
    CustomDSP::EQFilter f_SLF_5, f_SLF_6, f_SLF_7, f_SLF_8;
    
    CustomDSP::BUTLPFilter f_HLF_1, f_HLF_2, f_HLF_3, f_HLF_4;
    
    CustomDSP::EQFilter f_bump_1, f_bump_2;
    CustomDSP::EQFilter f_drop_1, f_drop_2;
    
    CustomDSP::EQFilter f_HF_1, f_HF_2;
    CustomDSP::EQFilter f_HF_bump_1, f_HF_bump_2;
    
    CustomDSP::EQFilter f_MF_bump_1, f_MF_bump_2;
    CustomDSP::EQFilter f_MF_1, f_MF_2;
    
    CustomDSP::EQFilter f_LF_bump_1, f_LF_bump_2;
    CustomDSP::EQFilter f_LF_drop_1, f_LF_drop_2;
    CustomDSP::EQFilter f_LF_1, f_LF_2;
    
    CustomDSP::LOPFilter f_pHF, f_pMF, f_pLF;
    
    double last_gain = 1.0;
    double hpf_gain_smooth = 0.0;
    double mpf_gain_smooth = 0.0;
    double lpf_gain_smooth = 0.0;
};
