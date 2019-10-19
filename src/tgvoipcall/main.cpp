#include <iostream>
#include <tgvoip/VoIPController.h>
#include <tgvoip/VoIPServerConfig.h>

#include "libs/libtgvoip/tests/MockReflector.h"
#include "libs/libtgvoip/webrtc_dsp/common_audio/wav_file.h"

#include <openssl/rand.h>

using namespace tgvoip;

int main (int argc, const char *argv []) {
    ServerConfig config;


    config.Update()
    return 0;
}


//
//int main (int argc, const char *argv []) {
//
//
//    auto testWavFilePath = argv[1];
//
//
//    test::MockReflector reflector("127.0.0.1", 1033);
//    reflector.Start();
//
//  //  for(int i=0;i<10;i++){
//        webrtc::WavReader wavReader(testWavFilePath);
//        webrtc::WavWriter wavWriter("output1.wav", 48000, 1);
//
//        VoIPController controller1;
//        VoIPController controller2;
//
//
//        std::array<std::array<uint8_t, 16>, 2> peerTags=test::MockReflector::GeneratePeerTags();
//        std::vector<Endpoint> endpoints1;
//        IPv4Address localhost("127.0.0.1");
//        IPv6Address emptyV6;
//        endpoints1.push_back(Endpoint(1, 1033, localhost, emptyV6, Endpoint::Type::UDP_RELAY, peerTags[0].data()));
//        controller1.SetRemoteEndpoints(endpoints1, false, 76);
//        std::vector<Endpoint> endpoints2;
//        endpoints2.push_back(Endpoint(1, 1033, localhost, emptyV6, Endpoint::Type::UDP_RELAY, peerTags[1].data()));
//        controller2.SetRemoteEndpoints(endpoints2, false, 76);
//
//        char encryptionKey[256];
//        RAND_bytes((uint8_t*)encryptionKey, sizeof(encryptionKey));
//        controller1.SetEncryptionKey(encryptionKey, true);
//        controller2.SetEncryptionKey(encryptionKey, false);
//
//
//        controller1.SetAudioDataCallbacks([&wavReader](int16_t* data, size_t len){
//            wavReader.ReadSamples(len, data);
//        }, [](int16_t* data, size_t len){
//
//        });
//
//        controller2.SetAudioDataCallbacks([](int16_t* data, size_t len){
//
//        }, [&wavWriter](int16_t* data, size_t len){
//            wavWriter.WriteSamples(data, len);
//        });
//
//;
//        controller1.Start();
//        controller2.Start();
//        controller1.Connect();
//        controller2.Connect();
//
//        usleep(13000000);
//    //    [NSThread sleepForTimeInterval:3.0];
//        controller1.Stop();
//        controller2.Stop();
//
//  //  }
//
//    reflector.Stop();
//    return 0;
//}