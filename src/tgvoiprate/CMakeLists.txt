cmake_minimum_required(VERSION 3.0)
project(tgvoiprate)

set(CMAKE_CXX_STANDARD 14)
file (GLOB_RECURSE FILES src/*.cpp)


add_executable(tgvoiprate ${FILES})