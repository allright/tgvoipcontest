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

int loopbackTest( const char *in, const char* out) {
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

    usleep(3000000);
  //  reflector.SetDropAllPackets(true);
    usleep(3000000);
  //  reflector.SetDropAllPackets(false);
    usleep(14000000);

    controller1.Stop();
    controller2.Stop();

    reflector.Stop();
    return 0;
}

int call(const char* reflector_port,
         const char* tag_caller_hex,
         const char* encryption_key_hex,
         const char* sound_in,
         const char* sound_out,
         const char* config,
         const char* role) {
    printf("%s> reflector:port = '%s'\n",role,reflector_port);
    printf("%s> tag_caller_hex = '%s'\n",role,tag_caller_hex);
    printf("%s> encryption_key_hex = '%s'\n",role,encryption_key_hex);
    printf("%s> sound_in = '%s'\n",role,sound_in);
    printf("%s> sound_out = '%s'\n",role,sound_out);
    printf("%s> config = '%s'\n",role,config);
    printf("%s> role = '%s'\n",role,role);
    usleep(3000000);
    printf("%s> fin\n",role);
    return 0;
}

int main (int argc, const char *argv []) {

    if (argc < 4) {
        printf("invalid number of args\n");
        return -1;
    }

    if (argv[1][0] == '-') {
        return loopbackTest( argv[2],argv[3]);
    } else {
        return call(argv[1],argv[2],argv[4],argv[6],argv[8],argv[10],argv[12]);
    }
}