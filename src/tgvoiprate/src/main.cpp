#include <vector>
#include <cmath>
#include "IPCMSource.h"
#include "LPF.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

static float compare(const char *origPath, const char *degPath);

int main(int argc, const char *argv[]) {

    if (argc < 3) {
        printf("usage: <original.ogg> <degraded.ogg> \n");
        return -1;
    }

    try {
        auto res = compare(argv[1], argv[2]);
        printf("%f\n", res);
        return 0;
    } catch (std::exception &ex) {
        printf("Exception: '%s'", ex.what());
        return -1;
    }

}


static short maxValue(const short *in, unsigned long nSamples) {
    short max = 0;
    for (auto i = 0; i < nSamples; i++) {
        short v = abs(*in++);
        max = max(max, v);
    }
    return max;
}

static float normalizeMult(const short *in, unsigned long nSamples) {
    short maxVal = maxValue(in, nSamples);
    float mult = maxVal == 0 ? 0 : (32767.0 / maxVal);
    return mult;
}


static float compareBuffers(IPCMSource *orig,
                            IPCMSource *deg,
                            unsigned long shift) {
    auto origSamples = orig->Size();
    const short *pOrig = orig->Samples();

    auto degSamples = deg->Size();
    const short *pDeg = deg->Samples();

    pDeg += shift;
    degSamples -= shift;

    unsigned long minSamples = min(origSamples, degSamples);

    LPF lpfOrig;
    LPF lpfDeg;
    // printf("orig: %lu samples, deg: %lu samples, minSamples: %lu\n", origSamples, degSamples, minSamples);

    float origMult = normalizeMult(pOrig, minSamples);
    float degMult = normalizeMult(pDeg, minSamples);
    //  printf("orig max: %f, deg max: %f\n", normalizeMult(pOrig, minSamples), normalizeMult(pDeg, minSamples));

    double errSum = 0;
    for (unsigned long i = 0; i < minSamples; i++) {
        short o = *pOrig++;
        short d = *pDeg++;

        float oLpf = lpfOrig.ProcessSample(origMult * abs(o));
        float dLpf = lpfDeg.ProcessSample(degMult * abs(d));

        float err = 1.0;
        if (oLpf > 0.000001 || dLpf > 0.000001) {
            err = dLpf > oLpf ? dLpf / oLpf : oLpf / dLpf;
            if (err > 50.0) {
                err = 50.0;
            }
        }

        errSum += err;
    }

    auto errorRes = errSum / minSamples;    // 1...25
    auto errorResMapped = (errorRes * errorRes - 1.0) / (25.0 * 25.0) * 4.0 + 1.0;

    if (errorResMapped > 5.0)
        errorResMapped = 5.0;
    return 6.0 - errorResMapped;
}

static float compare(const char *origPath, const char *degPath) {

    auto orig = IPCMSource::OpenWavFile(origPath);
    auto deg = IPCMSource::OpenWavFile(degPath);

    //   memset((void*)(deg->Samples() + 48000*2),16000,2*48000*2);
    IPCMSource *first;
    IPCMSource *second;

    long deltaSize = orig->Size() - deg->Size();

    if (deltaSize > 0) {
        second = orig.get();
        first = deg.get();
    } else {
        deltaSize = -deltaSize;
        second = deg.get();
        first = orig.get();
    }

    if (deltaSize == 0) {
        return compareBuffers(first, second, 0);
    }
   // printf("deltaSize = %ld\n", deltaSize);

    float rate = 1.0;
    auto bestShift = 0;
    // use shift step of one packet size 20ms at 48000, to figure out the best shift
    // shifting during 3 seconds!
    for (unsigned long shift = 0; shift < 48000 * 3; shift += 960) {
        auto currentRate = compareBuffers(first, second, shift);
        //  printf("shift = %f currentRate = %f rate = %f\n", shift / 48000.0, currentRate, rate);

        bool best = currentRate > rate;
        if (best) {
            rate = currentRate;
            bestShift = shift;
        }
    }
   // printf("bestShift = %u samples (%f sec) ", bestShift, bestShift / 48000.0);
    return rate;
}
