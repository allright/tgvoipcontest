#include <iostream>
#include <tgvoip/VoIPController.h>
#include <tgvoip/VoIPServerConfig.h>

#include "libs/libtgvoip/tests/MockReflector.h"
#include "libs/libtgvoip/webrtc_dsp/common_audio/wav_file.h"
#include <IPCMSource.h>
#include <IPCMDest.h>

#include <openssl/rand.h>
#include <algorithm>
using namespace tgvoip;

class PCMReader {
public:
    PCMReader(IPCMSource& source) {
        _to = source.Samples();
        _end = _to + source.Size();
    }

    size_t ReadSamples(size_t num_samples, int16_t* samples) {
        size_t left = _end - _to;
        unsigned long to_read = std::min(num_samples,left);
        memcpy(samples,_to,to_read*2);
        _to += to_read;
        return to_read;
    }

private:
    const short* _to;
    const short* _end;
};

class PCMWriter {
public:
    PCMWriter(IPCMDest& dest):_dest(dest) { }
    void WriteSamples(const int16_t* samples, size_t num_samples) {
        _dest.Write(samples,num_samples);
    }

private:
    IPCMDest& _dest;
};

void loopbackTest( const char *in, const char* out) {
    auto orig = IPCMSource::openOggFile(in, 48000);
    auto dest = IPCMDest::openOggWriter(out);

    test::MockReflector reflector("127.0.0.1", 1033);
    reflector.Start();

    PCMReader wavReader(*orig);
    PCMWriter wavWriter(*dest);

    VoIPController controller1;
    VoIPController controller2;


    std::array<std::array<uint8_t, 16>, 2> peerTags=test::MockReflector::GeneratePeerTags();
    std::vector<Endpoint> endpoints1;
    IPv4Address localhost("127.0.0.1");
    IPv6Address emptyV6;
    endpoints1.push_back(Endpoint(1, 1033, localhost, emptyV6, Endpoint::Type::UDP_RELAY, peerTags[0].data()));
    controller1.SetRemoteEndpoints(endpoints1, false, 76);
    std::vector<Endpoint> endpoints2;
    endpoints2.push_back(Endpoint(1, 1033, localhost, emptyV6, Endpoint::Type::UDP_RELAY, peerTags[1].data()));
    controller2.SetRemoteEndpoints(endpoints2, false, 76);

    char encryptionKey[256];
    RAND_bytes((uint8_t*)encryptionKey, sizeof(encryptionKey));
    controller1.SetEncryptionKey(encryptionKey, true);
    controller2.SetEncryptionKey(encryptionKey, false);

    controller1.SetAudioDataCallbacks([&wavReader](int16_t* data, size_t len){
        wavReader.ReadSamples(len, data);
    }, [](int16_t* data, size_t len){

    });

    controller2.SetAudioDataCallbacks([](int16_t* data, size_t len){

    }, [&wavWriter](int16_t* data, size_t len){
        wavWriter.WriteSamples(data, len);
    });

    controller1.Start();
    controller2.Start();
    usleep(1000000);
    controller1.Connect();
    controller2.Connect();

    usleep(20000000);

    controller1.Stop();
    controller2.Stop();

    reflector.Stop();
}

int main (int argc, const char *argv []) {

    if (argc < 4) {
        printf("invalid number of args\n");
        return -1;
    }

    if (argv[1][0] == '-') {
        loopbackTest( argv[2],argv[3]);
    }


    return 0;
}