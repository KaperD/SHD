#include <iostream>
#include <fstream>

#include "MurmurHash3/MurmurHash3.h"
#include "xxHash/xxhash.h"
#include "City/City.h"
#include "smhasher/src/SpeedTest.h"

void wrapperSity(const void * blob, const int len, const uint32_t seed, void * out) {
    uint128 s;
    s.first = seed;
    s.second = 0;
    uint128 result = CityHash128WithSeed((const char*)blob, len, s);
    auto* o = (uint64_t*) out;
    o[0] = result.second;
    o[1] = result.first;
}

void wrapperXXH(const void * blob, const int len, const uint32_t seed, void * out) {
    auto result = XXH3_128bits_withSeed(blob, len, seed);
    auto* o = (uint64_t*) out;
    o[0] = result.high64;
    o[1] = result.low64;
}

double test(pfHash hash, uint32_t current_byte) {
    double result = 0;
    int cycles = 10;
    if (current_byte > 1000) {
        cycles = 1;
    }
    for (int k = 0; k < cycles; ++k) {
        double x;
        TinySpeedTest(hash, 128, (int)current_byte, 42, false, x);
        result += x;
    }

    return result / cycles;
}

int main(int argc, char* argv[]) {
    (void) argc;
    uint32_t start_byte = atoll(argv[1]);
    uint32_t end_byte = atoll(argv[2]);
    uint32_t step_byte = atoll(argv[3]);

    std::ofstream os("out.csv");

    os << "bytes,mmh3,city,xxhash\n";

    for (uint32_t current_byte = start_byte; current_byte <= end_byte; current_byte += step_byte) {
        std::cout << current_byte << std::endl;
        double result;

        os << current_byte << ",";

        result = test(&MurmurHash3_x64_128, current_byte);
        os << result << ",";

        result = test(&wrapperSity, current_byte);
        os << result << ",";

        result = test(&wrapperXXH, current_byte);
        os << result << "\n";
    }

    return 0;
}
