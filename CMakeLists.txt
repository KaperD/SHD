cmake_minimum_required(VERSION 3.10)
project(SHD)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        main main.cpp
        MurmurHash3/MurmurHash3.cpp
        MurmurHash3/MurmurHash3.h
        xxHash/xxhash.h
        xxHash/xxhash.c
        City/City.h
        City/City.cpp
        smhasher/src/SpeedTest.cpp
)