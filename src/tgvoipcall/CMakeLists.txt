cmake_minimum_required(VERSION 3.0)
project(tgvoipcall)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWEBRTC_MAC")
    link_directories(/usr/local/Cellar/openssl@1.1/1.1.1d/lib/)
    set(LIBS libcrypto.a)
    include_directories(/usr/local/Cellar/openssl@1.1/1.1.1d/include/)
else()
    set(LIBS crypto pthread)
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWEBRTC_POSIX -DTGVOIP_USE_CALLBACK_AUDIO_IO")

include_directories(libs/libtgvoip/webrtc_dsp)


add_executable(tgvoipcall main.cpp ./libs/libtgvoip/tests/MockReflector.cpp)
target_link_libraries(tgvoipcall tgvoip.a ${LIBS})