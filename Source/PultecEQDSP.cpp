#include "PultecEQDSP.h"

const double PI = 3.14159265358979323846;

void PultecEQDSP::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    x1AA=x2AA=y1AA=y2AA=0;
    x1BA=x2BA=y1BA=y2BA=0;
    x1AB=x2AB=y1AB=y2AB=0;
    x1BB=x2BB=y1BB=y2BB=0;
    x1AC=x2AC=y1AC=y2AC=0;
    x1BC=x2BC=y1BC=y2BC=0;
    x1AD=x2AD=y1AD=y2AD=0;
    x1BD=x2BD=y1BD=y2BD=0;
    twetA = 0.0;
    twetB = 0.0;
}

void PultecEQDSP::process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params)
{
    double vslider2 = 20.0 * (params.low_boost / 10.0);
    double vslider3 = 16.0 * (params.low_atten / 10.0);
    double vslider77 = 18.0 * (params.peak_boost / 10.0);
    double vslider10 = 18.0 * (params.high_atten / 10.0);
    double vslider6 = 2.0 - (0.2 + (params.peak_q / 10.0) * 1.8);
    
    int vslider1 = params.lsf_freq;
    int vslider4 = params.peak_freq;
    int vslider7 = params.hsf_freq;
    
    vslider3 = -1.0 * vslider3;
    vslider10 = -1.0 * vslider10;
    
    double freqA = (vslider1 == 0 ? 3000.0 : (vslider1 == 1 ? 6000.0 : (vslider1 == 2 ? 12000.0 : 20000.0)));
    double fA = (3.0 / std::max(vslider2, 4.0)) * 2.0 * PI * std::min(freqA, 0.49 * sampleRate) / sampleRate;
    double qA = 0.08 * (36.0 - vslider2) / 18.0;
    
    double cos_w0A = std::cos(fA);
    double alphaA = std::sin(fA) / (2.0 * qA);
    
    double b1A = 1.0 - cos_w0A;
    double b0A = 0.5 * b1A;
    double b2A = b0A;
    double a0A = 1.0 + alphaA;
    double a1A = -2.0 * cos_w0A;
    double a2A = 1.0 - alphaA;
    
    a1A /= a0A; a2A /= a0A; b0A /= a0A; b1A /= a0A; b2A /= a0A;
    
    double tgt_wetA = std::pow(10.0, (0.9 * vslider2) / 20.0) - 1.0;
    
    double freqB = (vslider1 == 0 ? 4500.0 : (vslider1 == 1 ? 6500.0 : (vslider1 == 2 ? 11560.0 : 13640.0)));
    double fB = 2.0 * PI * std::min(freqB, 0.49 * sampleRate) / sampleRate;
    double qB = 0.12 * (36.0 - std::abs(vslider3)) / 18.0;
    
    double cos_w0B = std::cos(fB);
    double alphaB = std::sin(fB) / (2.0 * qB);
    
    double b1B = 1.0 - cos_w0B;
    double b0B = 0.5 * b1B;
    double b2B = b0B;
    double a0B = 1.0 + alphaB;
    double a1B = -2.0 * cos_w0B;
    double a2B = 1.0 - alphaB;
    
    a1B /= a0B; a2B /= a0B; b0B /= a0B; b1B /= a0B; b2B /= a0B;
    
    double tgt_wetB = 0.885 * std::log(1.0 - vslider3) / 2.9;
    
    double vslider5 = 3000.0;
    if (vslider4 == 1) vslider5 = 4000.0;
    else if (vslider4 == 2) vslider5 = 5000.0;
    else if (vslider4 == 3) vslider5 = 8000.0;
    else if (vslider4 == 4) vslider5 = 10000.0;
    else if (vslider4 == 5) vslider5 = 12000.0;
    else if (vslider4 == 6) vslider5 = 16000.0;
    
    double fC = 2.0 * PI * std::min(vslider5, 0.49 * sampleRate) / sampleRate;
    double qC = 0.3 + ((vslider6 - 0.2) * 0.3);
    double aC = std::pow(10.0, vslider77 / 40.0);
    double alphaC = std::sin(fC) / (2.0 * qC);
    
    double b0C = 1.0 + alphaC * aC;
    double b1C = -2.0 * std::cos(fC);
    double a1C = b1C;
    double b2C = 1.0 - alphaC * aC;
    double a0C = 1.0 + alphaC / aC;
    double a2C = 1.0 - alphaC / aC;
    
    a1C /= a0C; a2C /= a0C; b0C /= a0C; b1C /= a0C; b2C /= a0C;
    
    double freq2 = (vslider7 == 0 ? 3000.0 : (vslider7 == 1 ? 5000.0 : 9000.0)) / 2.0;
    double qD = (vslider7 == 0 ? 0.5 : (vslider7 == 1 ? 0.48 : 0.46));
    double fD = 2.0 * PI * std::min(freq2, 0.49 * sampleRate) / sampleRate;
    
    double tmp2 = vslider10;
    if (vslider7 > 1 && vslider10 < 0.0) tmp2 = vslider10 * 0.75;
    
    double aD = std::pow(10.0, tmp2 / 40.0);
    double cos_w0D = std::cos(fD);
    double tmp0D = 2.0 * std::sqrt(aD) * std::sin(fD) / (2.0 * qD);
    double tmp1D = (aD + 1.0) - (aD - 1.0) * cos_w0D;
    double tmp2D = (aD + 1.0) + (aD - 1.0) * cos_w0D;
    
    double b0D = aD * (tmp2D + tmp0D);
    double b1D = -2.0 * aD * ((aD - 1.0) + (aD + 1.0) * cos_w0D);
    double b2D = aD * (tmp2D - tmp0D);
    double a0D = tmp1D + tmp0D;
    double a1D = 2.0 * ((aD - 1.0) - (aD + 1.0) * cos_w0D);
    double a2D = tmp1D - tmp0D;
    
    a1D /= a0D; a2D /= a0D; b0D /= a0D; b1D /= a0D; b2D /= a0D;

    double d_wetA = (tgt_wetA - twetA) / numSamples;
    double d_wetB = (tgt_wetB - twetB) / numSamples;
    
    double trimScale = std::pow(10.0, params.trim / 20.0);

    for (int i = 0; i < numSamples; ++i)
    {
        twetA += d_wetA;
        twetB += d_wetB;
        
        double inA = leftChannel[i];
        double inB = rightChannel[i];
        
        double y0AA = b0A * inA + b1A * x1AA + b2A * x2AA - a1A * y1AA - a2A * y2AA;
        x2AA = x1AA; x1AA = inA; y2AA = y1AA; y1AA = y0AA;
        
        double y0BA = b0A * inB + b1A * x1BA + b2A * x2BA - a1A * y1BA - a2A * y2BA;
        x2BA = x1BA; x1BA = inB; y2BA = y1BA; y1BA = y0BA;
        
        double y0AB = b0B * inA + b1B * x1AB + b2B * x2AB - a1B * y1AB - a2B * y2AB;
        x2AB = x1AB; x1AB = inA; y2AB = y1AB; y1AB = y0AB;
        
        double y0BB = b0B * inB + b1B * x1BB + b2B * x2BB - a1B * y1BB - a2B * y2BB;
        x2BB = x1BB; x1BB = inB; y2BB = y1BB; y1BB = y0BB;
        
        double out0 = y0AA * twetA - y0AB * twetB + inA;
        double out1 = y0BA * twetA - y0BB * twetB + inB;
        
        double y0AC = b0C * out0 + b1C * x1AC + b2C * x2AC - a1C * y1AC - a2C * y2AC;
        x2AC = x1AC; x1AC = out0; y2AC = y1AC; y1AC = y0AC;
        
        double y0BC = b0C * out1 + b1C * x1BC + b2C * x2BC - a1C * y1BC - a2C * y2BC;
        x2BC = x1BC; x1BC = out1; y2BC = y1BC; y1BC = y0BC;
        
        double y0AD = b0D * y0AC + b1D * x1AD + b2D * x2AD - a1D * y1AD - a2D * y2AD;
        x2AD = x1AD; x1AD = y0AC; y2AD = y1AD; y1AD = y0AD;
        
        double y0BD = b0D * y0BC + b1D * x1BD + b2D * x2BD - a1D * y1BD - a2D * y2BD;
        x2BD = x1BD; x1BD = y0BC; y2BD = y1BD; y1BD = y0BD;
        
        double spl0 = y0AD * trimScale;
        double spl1 = y0BD * trimScale;
        
        spl0 = std::sin(spl0 * 0.258209) / 0.25535;
        spl1 = std::sin(spl1 * 0.258209) / 0.25535;

        leftChannel[i] = static_cast<float>(spl0);
        rightChannel[i] = static_cast<float>(spl1);
        
        double maxAbs = std::max(std::abs(spl0), std::abs(spl1));
        currentRMS = currentRMS * 0.99f + static_cast<float>(maxAbs) * 0.01f;
    }
}
