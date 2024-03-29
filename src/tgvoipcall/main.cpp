#include <iostream>
#include <tgvoip/VoIPController.h>
#include <tgvoip/VoIPServerConfig.h>

#include "libs/libtgvoip/tests/MockReflector.h"
#include "libs/libtgvoip/webrtc_dsp/rtc_base/logging.h"

#include <IPCMSource.h>
#include <IPCMDest.h>

#include <openssl/rand.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <condition_variable>

using namespace tgvoip;

class PCMReader {
public:
    PCMReader(IPCMSource &source) {
        _to = source.Samples();
        _end = _to + source.Size();
    }

    size_t ReadSamples(size_t num_samples, int16_t *samples) {
        size_t left = _end - _to;
        unsigned long to_read = std::min(num_samples, left);
        memcpy(samples, _to, to_read * 2);
        _to += to_read;
        return to_read;
    }

private:
    const short *_to;
    const short *_end;
};

class PCMWriter {
public:
    PCMWriter(IPCMDest &dest) : _dest(dest) {}

    void WriteSamples(const int16_t *samples, size_t num_samples) {
        _dest.Write(samples, num_samples);
    }

private:
    IPCMDest &_dest;
};

int loopbackTest(const char *in, const char *out) {
    auto orig = IPCMSource::openOggFile(in, 48000);
    auto dest = IPCMDest::openOggWriter(out);

    test::MockReflector reflector("127.0.0.1", 1033);
    reflector.Start();

    PCMReader wavReader(*orig);
    PCMWriter wavWriter(*dest);

    VoIPController controller1;
    VoIPController controller2;


    std::array<std::array<uint8_t, 16>, 2> peerTags = test::MockReflector::GeneratePeerTags();
    std::vector<Endpoint> endpoints1;
    IPv4Address localhost("127.0.0.1");
    IPv6Address emptyV6;
    endpoints1.push_back(Endpoint(1, 1033, localhost, emptyV6, Endpoint::Type::UDP_RELAY, peerTags[0].data()));
    controller1.SetRemoteEndpoints(endpoints1, false, 76);
    std::vector<Endpoint> endpoints2;
    endpoints2.push_back(Endpoint(1, 1033, localhost, emptyV6, Endpoint::Type::UDP_RELAY, peerTags[1].data()));
    controller2.SetRemoteEndpoints(endpoints2, false, 76);

    char encryptionKey[256];
    RAND_bytes((uint8_t *) encryptionKey, sizeof(encryptionKey));
    controller1.SetEncryptionKey(encryptionKey, true);
    controller2.SetEncryptionKey(encryptionKey, false);

    controller1.SetAudioDataCallbacks([&wavReader](int16_t *data, size_t len) {
        wavReader.ReadSamples(len, data);
    }, [](int16_t *data, size_t len) {

    });

    controller2.SetAudioDataCallbacks([](int16_t *data, size_t len) {

    }, [&wavWriter](int16_t *data, size_t len) {
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

static vector<string> split(const string &str, const string &delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

static unsigned char parse_hex(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    std::abort();
}

static std::vector<unsigned char> parse_string(const std::string &s) {
    if (s.size() % 2 != 0) std::abort();
    std::vector<unsigned char> result(s.size() / 2);

    for (std::size_t i = 0; i != s.size() / 2; ++i)
        result[i] = 16 * parse_hex(s[2 * i]) + parse_hex(s[2 * i + 1]);

    return result;
}

template<typename T>
void decodeHex(T &res, const std::string &source, unsigned len) {
    auto binary = parse_string(source);
    std::copy(binary.begin(), binary.begin() + len, res.begin());
}


int call(const char *reflector_port,
         const char *tag_hex,
         const char *encryption_key_hex,
         const char *sound_in,
         const char *sound_out,
         const char *config,
         const char *role,
         const char *id) {
//
//    printf("%s> tag_hex = '%s'\n", role, tag_hex);
//    printf("%s> encryption_key_hex = '%s'\n", role, encryption_key_hex);
//    printf("%s> sound_in = '%s'\n", role, sound_in);
//    printf("%s> sound_out = '%s'\n", role, sound_out);
//    printf("%s> config = '%s'\n", role, config);
//    printf("%s> role = '%s'\n", role, role);

    auto ref = split(reflector_port, ":");
    if (ref.size() != 2) {
        printf("%s> can not parse reflector:port'\n", role);
        return -1;
    }
    auto ip = ref[0];
    auto port = ref[1];
    uint16_t p = std::atoi(port.c_str());

    uint64_t iId = 1;
    if (strlen(id) > 0) {
        std::istringstream iss(id);
        iss >> iId;
    }

//    printf("%s> Id = '%s:%llu'\n", role, id, iId);
//    printf("%s> reflector:port = '%s:%u'\n", role, ip.c_str(), p);

    std::ifstream configFile;
    configFile.open(config); //open the input file

    std::stringstream strStream;
    strStream << configFile.rdbuf(); //read the file
    std::string configStr = strStream.str(); //configStr holds the content of the file

//    printf("%s> config:%s = '%s'\n", role, config, configStr.c_str());
    ServerConfig::GetSharedInstance()->Update(configStr);

    std::array<uint8_t, 16> s_tag_hex;
    decodeHex(s_tag_hex, tag_hex, 16);

    std::array<uint8_t, 256> s_encription_key;
    decodeHex(s_encription_key, encryption_key_hex, 256);

    bool isOutgoing = std::string(role) == std::string("caller");

    std::vector<Endpoint> endpoints;
    IPv4Address address(ip);
    IPv6Address emptyV6;

    endpoints.push_back(Endpoint(iId, p, address, emptyV6, Endpoint::Type::UDP_RELAY, s_tag_hex.data()));

    VoIPController controller;

    controller.SetRemoteEndpoints(endpoints, false, 76);
    controller.SetEncryptionKey((char *) s_encription_key.data(), isOutgoing);

    auto snd_in = IPCMSource::openOggFile(sound_in, 48000);
    auto snd_out = IPCMDest::openOggWriter(sound_out);

    PCMReader sndInReader(*snd_in);
    PCMWriter sndOutWriter(*snd_out);

    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::condition_variable started;
    std::condition_variable stopped;

    controller.SetAudioDataCallbacks([&](int16_t *data, size_t len) {
        started.notify_all();
        auto r = sndInReader.ReadSamples(len, data);
        if (r < len)
            stopped.notify_all();
    }, [&sndOutWriter](int16_t *data, size_t len) {
        sndOutWriter.WriteSamples(data, len);
    });

//    printf("%s> start \n", role);
    controller.Start();
    usleep(2000000);   // protect from "wrong fingerprint"
//    printf("%s> connect \n", role);
    controller.Connect();

    auto wait_result = started.wait_for(lock, std::chrono::seconds(5));
    switch (wait_result) {
        case cv_status::no_timeout:
            break;
        case cv_status::timeout:
            fprintf(stderr, "error connecting with params:\n");
            fprintf(stderr, "%s> tag_hex = '%s'\n", role, tag_hex);
            fprintf(stderr, "%s> encryption_key_hex = '%s'\n", role, encryption_key_hex);
            fprintf(stderr, "%s> sound_in = '%s'\n", role, sound_in);
            fprintf(stderr, "%s> sound_out = '%s'\n", role, sound_out);
            fprintf(stderr, "%s> config = '%s'\n", role, config);
            fprintf(stderr, "%s> role = '%s'\n", role, role);
            controller.Stop();
            return -1;
    }

//    printf("%s> started \n", role);
    stopped.wait(lock);

//    printf("%s> stopping \n", role);
    usleep(3000000);
    controller.Stop();
    //   printf("%s> fin res = '%s'\n", role, controller.GetDebugLog().c_str());
    printf("%s\n", controller.GetDebugLog().c_str());
    return 0;
}

std::map<std::string, std::string> parseOptions(int argc, const char *argv[]) {
    std::map<std::string, std::string> options;
    for (auto i = 0; i < argc; i++) {
        auto key = argv[i];
        if (key[0] == '-') {
            i++;
            if (i < argc) {
                options[key] = argv[i];
            }
        }
    }
    return options;
}

int main(int argc, const char *argv[]) {
    if (argc < 4) {
        printf("invalid number of args\n");
        return -1;
    }

    rtc::LogMessage::LogToDebug(rtc::LS_NONE);

    if (argv[1][0] == '-') {
        return loopbackTest(argv[2], argv[3]);
    } else {
        if (argc < 13) {
            printf("invalid number of args\n");
            return -1;
        }
        auto r = parseOptions(argc, argv);
        return call(argv[1],
                    argv[2],
                    r["-k"].c_str(),
                    r["-i"].c_str(),
                    r["-o"].c_str(),
                    r["-c"].c_str(),
                    r["-r"].c_str(),
                    r["-id"].c_str());
    }
}