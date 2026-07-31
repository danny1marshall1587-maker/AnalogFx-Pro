#include "FairchildCompressor.h"

void FairchildCompressor::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    lrunave = 0.0;
    lrundb = 0.0;
    rrunave = 0.0;
    rrundb = 0.0;
}

void FairchildCompressor::process(float* leftChannel, float* rightChannel, int numSamples, const Parameters& params)
{
    double lthreshv = std::exp(params.l_thresh * db2log);
    double lratio = 20.0;
    double lbias = 80.0 * params.l_bias / 100.0;
    double lmakeupv = std::exp(params.l_makeup * db2log);

    double rthreshv = std::exp(params.r_thresh * db2log);
    double rratio = 20.0;
    double rbias = 80.0 * params.r_bias / 100.0;
    double rmakeupv = std::exp(params.r_makeup * db2log);

    double lattime = 0.0002, lreltime = 0.300;
    if (params.l_time == 2) { lattime = 0.0002; lreltime = 0.800; }
    else if (params.l_time == 3) { lattime = 0.0004; lreltime = 2.000; }
    else if (params.l_time == 4) { lattime = 0.0008; lreltime = 5.000; }
    else if (params.l_time == 5) { lattime = 0.0002; lreltime = 10.000; }
    else if (params.l_time == 6) { lattime = 0.0004; lreltime = 25.000; }
    
    double latcoef = std::exp(-1.0 / (lattime * sampleRate));
    double lrelcoef = std::exp(-1.0 / (lreltime * sampleRate));
    
    double lrmstime = params.l_rms / 1000000.0;
    double lrmscoef = std::exp(-1.0 / (lrmstime * sampleRate));

    double rattime = 0.0002, rreltime = 0.300;
    if (params.r_time == 2) { rattime = 0.0002; rreltime = 0.800; }
    else if (params.r_time == 3) { rattime = 0.0004; rreltime = 2.000; }
    else if (params.r_time == 4) { rattime = 0.0008; rreltime = 5.000; }
    else if (params.r_time == 5) { rattime = 0.0002; rreltime = 10.000; }
    else if (params.r_time == 6) { rattime = 0.0004; rreltime = 25.000; }

    double ratcoef = std::exp(-1.0 / (rattime * sampleRate));
    double rrelcoef = std::exp(-1.0 / (rreltime * sampleRate));
    
    double rrmstime = params.r_rms / 1000000.0;
    double rrmscoef = std::exp(-1.0 / (rrmstime * sampleRate));

    double capsc = log2db;

    for (int i = 0; i < numSamples; ++i)
    {
        double spl0 = leftChannel[i];
        double spl1 = rightChannel[i];

        if (params.agc == 0) // Left/Right (Linked)
        {
            double aspl0 = std::abs(spl0);
            double aspl1 = std::abs(spl1);
            double lmaxspl = std::max(aspl0, aspl1);
            lmaxspl = lmaxspl * lmaxspl;

            lrunave = lmaxspl + lrmscoef * (lrunave - lmaxspl);
            double ldet = std::sqrt(std::max(0.0, lrunave));

            double loverdb = capsc * std::log(std::max(1e-10, ldet / lthreshv));
            loverdb = std::max(0.0, loverdb);

            if (loverdb > lrundb)
                lrundb = loverdb + latcoef * (lrundb - loverdb);
            else
                lrundb = loverdb + lrelcoef * (lrundb - loverdb);
            
            loverdb = std::max(lrundb, 0.0);

            double lcratio = lratio;
            if (lbias != 0.0)
                lcratio = 1.0 + (lratio - 1.0) * std::sqrt((loverdb + dcoffset) / (lbias + dcoffset));
            
            double lgr_db = -loverdb * (lcratio - 1.0) / lcratio;
            currentGR = static_cast<float>(lgr_db);
            double lgrv = std::exp(lgr_db * db2log);

            spl0 *= lgrv * lmakeupv;
            spl1 *= lgrv * lmakeupv;
        }
        else // Lat/Vert (Mid/Side)
        {
            double aspl0 = std::abs(spl0 + spl1) / 2.0;
            double aspl1 = std::abs(spl0 - spl1) / 2.0;
            
            double lmaxspl = aspl0 * aspl0;
            lrunave = lmaxspl + lrmscoef * (lrunave - lmaxspl);
            double ldet = std::sqrt(std::max(0.0, lrunave));

            double loverdb = capsc * std::log(std::max(1e-10, ldet / lthreshv));
            loverdb = std::max(0.0, loverdb);

            if (loverdb > lrundb)
                lrundb = loverdb + latcoef * (lrundb - loverdb);
            else
                lrundb = loverdb + lrelcoef * (lrundb - loverdb);
            
            loverdb = std::max(lrundb, 0.0);

            double lcratio = lratio;
            if (lbias != 0.0)
                lcratio = 1.0 + (lratio - 1.0) * std::sqrt((loverdb + dcoffset) / (lbias + dcoffset));

            double rmaxspl = aspl1 * aspl1;
            rrunave = rmaxspl + rrmscoef * (rrunave - rmaxspl);
            double rdet = std::sqrt(std::max(0.0, rrunave));

            double roverdb = capsc * std::log(std::max(1e-10, rdet / rthreshv));
            roverdb = std::max(0.0, roverdb);

            if (roverdb > rrundb)
                rrundb = roverdb + ratcoef * (rrundb - roverdb);
            else
                rrundb = roverdb + rrelcoef * (rrundb - roverdb);
            
            roverdb = std::max(rrundb, 0.0);

            double rcratio = rratio;
            if (rbias != 0.0)
                rcratio = 1.0 + (rratio - 1.0) * std::sqrt((roverdb + dcoffset) / (rbias + dcoffset));

            double lgr_db = -loverdb * (lcratio - 1.0) / lcratio;
            double rgr_db = -roverdb * (rcratio - 1.0) / rcratio;
            currentGR = static_cast<float>(std::min(lgr_db, rgr_db)); // Most negative is most GR

            double lgrv = std::exp(lgr_db * db2log);
            double rgrv = std::exp(rgr_db * db2log);

            double sav0 = (spl0 + spl1) * lgrv;
            double sav1 = (spl0 - spl1) * rgrv;
            sav0 *= lmakeupv;
            sav1 *= rmakeupv;
            
            spl0 = (sav0 + sav1) * 0.5;
            spl1 = (sav0 - sav1) * 0.5;
        }

        leftChannel[i] = static_cast<float>(spl0);
        rightChannel[i] = static_cast<float>(spl1);
        
        double maxAbs = std::max(std::abs(spl0), std::abs(spl1));
        currentRMS = currentRMS * 0.999f + static_cast<float>(maxAbs) * 0.001f;
    }
}
