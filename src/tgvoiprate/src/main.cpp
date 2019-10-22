#include <IPCMSource.h>

extern "C" float pesq(short* orig, unsigned long origSample, short* deg, unsigned long degSamples);

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


static float compare(const char *origPath, const char *degPath) {
    auto orig = IPCMSource::openOggFile(origPath);//std::shared_ptr<IPCMSource>(new File(origPath));
    auto deg =  IPCMSource::openOggFile(degPath);//std::shared_ptr<IPCMSource>(new File(degPath));
   // printf("bestShift = %u samples (%f sec) ", bestShift, bestShift / 48000.0);
    return pesq((short *)orig->Samples(),orig->Size(),(short *)deg->Samples(),deg->Size());
}
