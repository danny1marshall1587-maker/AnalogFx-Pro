#include "NeveEQDSP.h"

void NeveEQDSP::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    
    f_in_1.setFreq(11, sampleRate); f_in_2.setFreq(11, sampleRate);
    f_in_3.setFreq(5, sampleRate); f_in_4.setFreq(5, sampleRate);
    
    f_HLF_1.setFreq(20843, sampleRate); f_HLF_2.setFreq(20843, sampleRate);
    f_HLF_3.setFreq(20843, sampleRate); f_HLF_4.setFreq(20843, sampleRate);
    
    f_bump_1.setParams(850, 0.5, 0.38, 0, sampleRate);
    f_bump_2.setParams(850, 0.5, 0.38, 0, sampleRate);
    
    f_drop_1.setParams(18300, 0.3, -0.8, 0, sampleRate);
    f_drop_2.setParams(18300, 0.3, -0.8, 0, sampleRate);
    
    f_pHF.setFreq(10, sampleRate);
    f_pMF.setFreq(10, sampleRate);
    f_pLF.setFreq(10, sampleRate);
    
    last_gain = 1.0;
}

void NeveEQDSP::process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params)
{
    double next_gain = std::pow(10.0, params.gain / 20.0);
    double d_gain = (next_gain - last_gain) / numSamples;
    
    double sat_S = 10.0 * std::exp((2.0 * (params.drive / 10.0) - 20.0) / 2.71828);
    double atan_S = std::atan(sat_S);
    double sat_trim = (1.0 - 0.5 * atan_S);
    double bias = 0.0021;
    
    hpf_gain_smooth = params.hf_gain;
    mpf_gain_smooth = params.mf_gain;
    lpf_gain_smooth = params.lf_gain;
    
    if (params.hpf_freq > 0) {
        double f_cut=0, b1=0, g1=0, q1=0, b2=0, g2=0, q2=0;
        if (params.hpf_freq == 1) { f_cut=34; b1=46; g1=1.1; q1=1.6; b2=78; g2=-0.3; q2=0.7; }
        else if (params.hpf_freq == 2) { f_cut=64; b1=76; g1=2.4; q1=1.2; b2=110; g2=-1.6; q2=0.7; }
        else if (params.hpf_freq == 3) { f_cut=132; b1=160; g1=2.2; q1=1.9; b2=302; g2=-0.5; q2=1; }
        else if (params.hpf_freq == 4) { f_cut=270; b1=330; g1=2.4; q1=1.6; b2=665; g2=-0.4; q2=0.7; }
        
        f_SLF_1.setFreq(f_cut, sampleRate); f_SLF_2.setFreq(f_cut, sampleRate);
        f_SLF_3.setFreq(f_cut, sampleRate); f_SLF_4.setFreq(f_cut, sampleRate);
        f_SLF_5.setParams(b1, q1, g1, 0, sampleRate); f_SLF_6.setParams(b1, q1, g1, 0, sampleRate);
        f_SLF_7.setParams(b2, q2, g2, 0, sampleRate); f_SLF_8.setParams(b2, q2, g2, 0, sampleRate);
    }
    
    f_HF_1.setParams(2000, 0.35, hpf_gain_smooth, 2, sampleRate);
    f_HF_2.setParams(2000, 0.35, hpf_gain_smooth, 2, sampleRate);
    f_HF_bump_1.setParams(820, 0.45, hpf_gain_smooth * -3.5 / 18.0, 0, sampleRate);
    f_HF_bump_2.setParams(820, 0.45, hpf_gain_smooth * -3.5 / 18.0, 0, sampleRate);
    
    if (params.mf_freq > 0) {
        double f_cut=0, f_main=0, f_q=0;
        if (params.mf_freq == 1) { f_cut=380; f_main=358; f_q=0.22+std::abs(mpf_gain_smooth/60.0); }
        else if (params.mf_freq == 2) { f_cut=700; f_main=750; f_q=0.22+std::abs(mpf_gain_smooth/55.0); }
        else if (params.mf_freq == 3) { f_cut=1600; f_main=1590; f_q=0.22+std::abs(mpf_gain_smooth/60.0); }
        else if (params.mf_freq == 4) { f_cut=3200; f_main=3200; f_q=0.22+std::abs(mpf_gain_smooth/72.0); }
        else if (params.mf_freq == 5) { f_cut=6800; f_main=5800-600.0*std::abs(mpf_gain_smooth)/18.0; f_q=0.22+std::abs(mpf_gain_smooth/35.0); }
        else if (params.mf_freq == 6) { f_cut=12200; f_main=9400-2130.0*std::abs(mpf_gain_smooth)/18.0; f_q=0.22+std::abs(mpf_gain_smooth/32.0); }
        
        f_MF_bump_1.setParams(f_cut, 0.25, 1.0, 0, sampleRate);
        f_MF_bump_2.setParams(f_cut, 0.25, 1.0, 0, sampleRate);
        f_MF_1.setParams(f_main, f_q, mpf_gain_smooth, 0, sampleRate);
        f_MF_2.setParams(f_main, f_q, mpf_gain_smooth, 0, sampleRate);
    }
    
    if (params.lf_freq > 0) {
        double f_bump=0, f_drop=0, f_main=0, f_q=0;
        if (params.lf_freq == 1) { f_bump=35; f_drop=240; f_main=30; f_q=0.22; }
        else if (params.lf_freq == 2) { f_bump=80; f_drop=420; f_main=32; f_q=0.12; }
        else if (params.lf_freq == 3) { f_bump=130; f_drop=540; f_main=50; f_q=0.12; }
        else if (params.lf_freq == 4) { f_bump=240; f_drop=1000; f_main=80; f_q=0.12; }
        
        double bump_boost = 0.5 + params.lf_freq / 8.0;
        f_LF_bump_1.setParams(f_bump, 0.25, bump_boost, 0, sampleRate);
        f_LF_bump_2.setParams(f_bump, 0.25, bump_boost, 0, sampleRate);
        
        double drop_f = f_drop + std::abs(lpf_gain_smooth * 5.0);
        double drop_boost = -0.5 - f_bump/11.0 - lpf_gain_smooth / (2.6 + 2.0/f_bump);
        f_LF_drop_1.setParams(drop_f, 0.5, drop_boost, 0, sampleRate);
        f_LF_drop_2.setParams(drop_f, 0.5, drop_boost, 0, sampleRate);
        
        f_LF_1.setParams(f_main, f_q, lpf_gain_smooth, 0, sampleRate);
        f_LF_2.setParams(f_main, f_q, lpf_gain_smooth, 0, sampleRate);
    }

    double trimScale = std::pow(10.0, params.trim / 20.0);

    for (int i = 0; i < numSamples; ++i)
    {
        double spl0 = leftChannel[i];
        double spl1 = rightChannel[i];

        spl0 = f_in_1.process(spl0);
        spl1 = f_in_2.process(spl1);

        if (params.phase) {
            spl0 *= -1.0;
            spl1 *= -1.0;
        }

        spl0 += (1.0 - (spl0 * spl0) / (1.0 + spl0 * spl0)) * bias * sat_S / 10.0;
        spl1 += (1.0 - (spl1 * spl1) / (1.0 + spl1 * spl1)) * bias * sat_S / 10.0;

        if (params.drive > 0.0) {
            spl0 = sat_trim * std::atan(spl0 * sat_S) / atan_S;
            spl1 = sat_trim * std::atan(spl1 * sat_S) / atan_S;
        }

        spl0 *= last_gain;
        spl1 *= last_gain;
        last_gain += d_gain;

        spl0 = f_in_3.process(spl0);
        spl1 = f_in_4.process(spl1);

        if (params.eq_on) {
            if (params.hpf_on && params.hpf_freq > 0) {
                spl0 = f_SLF_1.process(spl0);
                spl0 = f_SLF_3.process(spl0);
                spl1 = f_SLF_2.process(spl1);
                spl1 = f_SLF_4.process(spl1);
                spl0 = f_SLF_5.process(spl0);
                spl1 = f_SLF_6.process(spl1);
                spl0 = f_SLF_7.process(spl0);
                spl1 = f_SLF_8.process(spl1);
            }
 
            spl0 = f_bump_1.process(spl0);
            spl1 = f_bump_2.process(spl1);
            
            spl0 = f_drop_1.process(spl0);
            spl1 = f_drop_2.process(spl1);
 
            if (params.hf_on) {
                spl0 = f_HF_1.process(spl0);
                spl1 = f_HF_2.process(spl1);
                spl0 = f_HF_bump_1.process(spl0);
                spl1 = f_HF_bump_2.process(spl1);
            }
 
            if (params.mf_on && params.mf_freq > 0) {
                spl0 = f_MF_bump_1.process(spl0);
                spl1 = f_MF_bump_2.process(spl1);
                spl0 = f_MF_1.process(spl0);
                spl1 = f_MF_2.process(spl1);
            }
 
            if (params.lf_on && params.lf_freq > 0) {
                spl0 = f_LF_bump_1.process(spl0);
                spl1 = f_LF_bump_2.process(spl1);
                spl0 = f_LF_drop_1.process(spl0);
                spl1 = f_LF_drop_2.process(spl1);
                spl0 = f_LF_1.process(spl0);
                spl1 = f_LF_2.process(spl1);
            }
 
            if (params.hpf_on) {
                spl0 = f_HLF_1.process(spl0);
                spl0 = f_HLF_3.process(spl0);
                spl1 = f_HLF_2.process(spl1);
                spl1 = f_HLF_4.process(spl1);
            }
        }

        spl0 *= trimScale;
        spl1 *= trimScale;

        leftChannel[i] = static_cast<float>(spl0);
        rightChannel[i] = static_cast<float>(spl1);
        
        double maxAbs = std::max(std::abs(spl0), std::abs(spl1));
        currentRMS = currentRMS * 0.99f + static_cast<float>(maxAbs) * 0.01f;
    }
}
