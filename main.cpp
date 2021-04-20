#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>
#include <cstring>
#include <cassert>

#include "lz4.h"
#include "zstd.h"

enum Algorithm {
    LZ4,
    ZSTD1,
    ZSTD7,
};

void testCorrectness(std::filesystem::path& path, Algorithm algorithm) {
    std::ifstream is(path, std::ifstream::in | std::ifstream::binary);
    auto size = (int) file_size(path);
    char* data = static_cast<char *>(malloc(size));
    char* compressed_data = static_cast<char *>(malloc(size));
    char* decompressed_data = static_cast<char *>(malloc(size));
    is.read(data, size);
    if (algorithm == Algorithm::LZ4) {
        auto compressed_size = LZ4_compress_default(data, compressed_data, size, size);
        LZ4_decompress_safe(compressed_data, decompressed_data, compressed_size, size);
    } else if (algorithm == Algorithm::ZSTD1) {
        auto compressed_size = (int) ZSTD_compress(compressed_data, size, data, size, 1);
        ZSTD_decompress(decompressed_data, size, compressed_data, compressed_size);
    } else {
        auto compressed_size = (int) ZSTD_compress(compressed_data, size, data, size, 7);
        ZSTD_decompress(decompressed_data, size, compressed_data, compressed_size);
    }

    assert(memcmp(data, decompressed_data, size) == 0);

    free(data);
    free(compressed_data);
    free(decompressed_data);
}

struct Result {
    long long milliseconds;
    double compression_ratio;
};

Result testLZ4(char* data, char* compressed_data, int length) {
    std::clock_t begin = clock();
    int out_size = LZ4_compress_default(data, compressed_data, length, length);
    std::clock_t end = clock();

    return {
        ((end - begin) * 1000) / CLOCKS_PER_SEC,
        (double)length / out_size
    };
}

Result testZSTD1(char* data, char* compressed_data, int length) {
    std::clock_t begin = clock();
    std::size_t out_size = ZSTD_compress(compressed_data, length, data, length, 1);
    std::clock_t end = clock();

    return {
            ((end - begin) * 1000) / CLOCKS_PER_SEC,
            (double)length / (double)out_size
    };
}

Result testZSTD7(char* data, char* compressed_data, int length) {
    std::clock_t begin = clock();
    std::size_t out_size = ZSTD_compress(compressed_data, length, data, length, 7);
    std::clock_t end = clock();

    return {
            ((end - begin) * 1000) / CLOCKS_PER_SEC,
            (double)length / (double)out_size
    };
}

int main(int argc, char* argv[]) {
    std::ofstream speed("speed.csv");
    std::ofstream compression("compression.csv");

    constexpr std::string_view first_line = "filename,lz4,zstd_lvl1,zstd_lvl7";
    speed << first_line << '\n';
    compression << first_line << '\n';

    for (int k = 1; k < argc; ++k) {
        std::filesystem::path path(argv[k]);

//        testCorrectness(path, Algorithm::LZ4);
//        testCorrectness(path, Algorithm::ZSTD1);
//        testCorrectness(path, Algorithm::ZSTD7);

        speed << path.filename() << ',';
        compression << path.filename() << ',';
        int length = (int)file_size(path);
        char* data = static_cast<char *>(malloc(length));
        std::ifstream input(path, std::ifstream::in | std::ifstream::binary);
        input.read(data, length);
        char* compressed_data = static_cast<char *>(malloc(length));

        auto result = testLZ4(data, compressed_data, length);
        speed << result.milliseconds << ',';
        compression << result.compression_ratio << ',';

        result = testZSTD1(data, compressed_data, length);
        speed << result.milliseconds << ',';
        compression << result.compression_ratio << ',';

        result = testZSTD7(data, compressed_data, length);
        speed << result.milliseconds << '\n';
        compression << result.compression_ratio << '\n';

        free(compressed_data);
        free(data);
    }

    return 0;
}
