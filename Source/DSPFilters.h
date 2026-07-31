#pragma once

#include <cmath>
#include <algorithm>

namespace CustomDSP {

const double PI = 3.14159265358979323846;

class LOPFilter {
public:
    void setFreq(double freq, double sampleRate) {
        double ny = sampleRate * 0.49;
        freq = std::clamp(freq, 0.1, ny);
        c = std::exp(-2.0 * PI * freq / sampleRate);
    }
    double process(double signal) {
        y0 = signal + (y0 - signal) * c;
        return y0;
    }
private:
    double y0 = 0.0, c = 0.0;
};

class HIPFilter {
public:
    void setFreq(double freq, double sampleRate) {
        double ny = sampleRate * 0.49;
        freq = std::clamp(freq, 0.1, ny);
        b1 = std::exp(-2.0 * PI * freq / sampleRate);
        a0 = (1.0 + b1) / 2.0;
        a1 = -a0;
    }
    double process(double signal) {
        y0 = signal * a0 + x0 * a1 + y0 * b1;
        x0 = signal;
        return y0;
    }
private:
    double x0 = 0.0, y0 = 0.0, a0 = 0.0, a1 = 0.0, b1 = 0.0;
};

class BUTLPFilter {
public:
    void setFreq(double freq, double sampleRate) {
        double ny = sampleRate * 0.49;
        freq = std::clamp(freq, 1.0, ny);
        double pioversr = PI / sampleRate;
        double sqrt2 = std::sqrt(2.0);
        double c_ = 1.0 / std::tan(pioversr * freq);
        double c2_ = c_ * c_;
        a0 = 1.0 / (1.0 + sqrt2 * c_ + c2_);
        a2 = a0;
        a1 = 2.0 * a0;
        b1 = a1 * (1.0 - c2_);
        b2 = a0 * (1.0 - sqrt2 * c_ + c2_);
    }
    double process(double signal) {
        double tmp = a0 * signal + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
        x2 = x1; x1 = signal; y2 = y1; y1 = tmp;
        return tmp;
    }
private:
    double x1=0, x2=0, y1=0, y2=0, a0=0, a1=0, a2=0, b1=0, b2=0;
};

class BUTHPFilter {
public:
    void setFreq(double freq, double sampleRate) {
        double ny = sampleRate * 0.49;
        freq = std::clamp(freq, 1.0, ny);
        double pioversr = PI / sampleRate;
        double sqrt2 = std::sqrt(2.0);
        double c_ = std::tan(pioversr * freq);
        double c2_ = c_ * c_;
        a0 = 1.0 / (1.0 + sqrt2 * c_ + c2_);
        a2 = a0;
        a1 = -2.0 * a0;
        b1 = 2.0 * a0 * (c2_ - 1.0);
        b2 = a0 * (1.0 - sqrt2 * c_ + c2_);
    }
    double process(double signal) {
        double tmp = a0 * signal + a1 * x1 + a2 * x2 - b1 * y1 - b2 * y2;
        x2 = x1; x1 = signal; y2 = y1; y1 = tmp;
        return tmp;
    }
private:
    double x1=0, x2=0, y1=0, y2=0, a0=0, a1=0, a2=0, b1=0, b2=0;
};

class EQFilter {
public:
    void setParams(double freq, double q, double boost, int type, double sampleRate) {
        double ny = sampleRate * 0.49;
        freq = std::clamp(freq, 1.0, ny);
        q = std::max(0.1, q);
        type = std::clamp(type, 0, 2);
        
        double a = std::pow(10.0, boost / 40.0);
        double w0 = 2.0 * PI * freq / sampleRate;
        double c = std::cos(w0);
        double alpha = std::sin(w0) / (2.0 * q);
        
        if (type == 0) { // Peak
            double amul = alpha * a;
            double adiv = alpha / a;
            b0 = 1.0 + amul;
            b1 = -2.0 * c;
            a1 = b1;
            b2 = 1.0 - amul;
            a0 = 1.0 + adiv;
            a2 = 1.0 - adiv;
        } else if (type == 1) { // Low Shelf
            double sqrt2a = std::sqrt(a * 2.0) * alpha;
            double amin1c = (a - 1.0) * c;
            double aadd1c = (a + 1.0) * c;
            b0 = a * ((a + 1.0) - amin1c + sqrt2a);
            b1 = 2.0 * a * ((a - 1.0) - aadd1c);
            b2 = a * ((a + 1.0) - amin1c - sqrt2a);
            a0 = (a + 1.0) + amin1c + sqrt2a;
            a1 = -2.0 * ((a - 1.0) + aadd1c);
            a2 = (a + 1.0) + amin1c - sqrt2a;
        } else if (type == 2) { // High Shelf
            double sqrt2a = std::sqrt(a * 2.0) * alpha;
            double amin1c = (a - 1.0) * c;
            double aadd1c = (a + 1.0) * c;
            b0 = a * ((a + 1.0) + amin1c + sqrt2a);
            b1 = -2.0 * a * ((a - 1.0) + aadd1c);
            b2 = a * ((a + 1.0) + amin1c - sqrt2a);
            a0 = (a + 1.0) - amin1c + sqrt2a;
            a1 = 2.0 * ((a - 1.0) - aadd1c);
            a2 = (a + 1.0) - amin1c - sqrt2a;
        }
    }
    double process(double signal) {
        double tmp = (b0 * signal + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2) / a0;
        x2 = x1; x1 = signal; y2 = y1; y1 = tmp;
        return tmp;
    }
private:
    double x1=0, x2=0, y1=0, y2=0, a0=1, a1=0, a2=0, b0=1, b1=0, b2=0;
};

} // namespace CustomDSP
