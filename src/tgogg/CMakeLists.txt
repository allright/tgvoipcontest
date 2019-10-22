cmake_minimum_required(VERSION 3.0)
project(tgogg)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake-modules" CACHE STRING "Modules for CMake" FORCE)

message(${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL AVDEVICE REQUIRED) #add here the list of ffmpeg components required


find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(SWRESAMPLE_INCLUDE_DIR libavresample/swresample.h)
find_library(SWRESAMPLE_LIBRARY swresample)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

file (GLOB_RECURSE FILES src/*.c*)

add_library(tgogg ${FILES})


message(${AVUTIL_LIBRARY})
include_directories(../include)

target_include_directories(tgogg PRIVATE ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR})
target_link_libraries(tgogg PRIVATE ${SWRESAMPLE_LIBRARY} ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY})
