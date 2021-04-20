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
    auto data_size = (int) file_size(path);
    char* data = static_cast<char *>(malloc(data_size));
    char* compressed_data = static_cast<char *>(malloc(data_size));
    char* decompressed_data = static_cast<char *>(malloc(data_size));
    is.read(data, data_size);
    if (algorithm == Algorithm::LZ4) {
        auto compressed_size = LZ4_compress_default(data, compressed_data, data_size, data_size);
        LZ4_decompress_safe(compressed_data, decompressed_data, compressed_size, data_size);
    } else if (algorithm == Algorithm::ZSTD1) {
        auto compressed_size = (int) ZSTD_compress(compressed_data, data_size, data, data_size, 1);
        ZSTD_decompress(decompressed_data, data_size, compressed_data, compressed_size);
    } else {
        auto compressed_size = (int) ZSTD_compress(compressed_data, data_size, data, data_size, 7);
        ZSTD_decompress(decompressed_data, data_size, compressed_data, compressed_size);
    }

    assert(memcmp(data, decompressed_data, data_size) == 0);

    free(data);
    free(compressed_data);
    free(decompressed_data);
}

struct Result {
    long long milliseconds;
    double compression_ratio;
};

Result testLZ4(char* data, char* compressed_data, char* decompressed_data, int length) {
    std::clock_t begin = clock();
    int compressed_size = LZ4_compress_default(data, compressed_data, length, length);
    LZ4_decompress_safe(compressed_data, decompressed_data, compressed_size, length);
    std::clock_t end = clock();

    return {
        ((end - begin) * 1000) / CLOCKS_PER_SEC,
        (double)length / compressed_size
    };
}

Result testZSTD1(char* data, char* compressed_data, char* decompressed_data, int length) {
    std::clock_t begin = clock();
    std::size_t compressed_size = ZSTD_compress(compressed_data, length, data, length, 1);
    ZSTD_decompress(decompressed_data, length, compressed_data, compressed_size);
    std::clock_t end = clock();

    return {
            ((end - begin) * 1000) / CLOCKS_PER_SEC,
            (double)length / (double)compressed_size
    };
}

Result testZSTD7(char* data, char* compressed_data, char* decompressed_data, int length) {
    std::clock_t begin = clock();
    std::size_t compressed_size = ZSTD_compress(compressed_data, length, data, length, 7);
    ZSTD_decompress(decompressed_data, length, compressed_data, compressed_size);
    std::clock_t end = clock();

    return {
            ((end - begin) * 1000) / CLOCKS_PER_SEC,
            (double)length / (double)compressed_size
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

        if (k == 1) {
            testCorrectness(path, Algorithm::LZ4);
            testCorrectness(path, Algorithm::ZSTD1);
            testCorrectness(path, Algorithm::ZSTD7);
        }

        speed << path.filename() << ',';
        compression << path.filename() << ',';
        int data_size = (int)file_size(path);
        char* data = static_cast<char *>(malloc(data_size));
        std::ifstream input(path, std::ifstream::in | std::ifstream::binary);
        input.read(data, data_size);
        char* compressed_data = static_cast<char *>(malloc(data_size));
        char* decompressed_data = static_cast<char *>(malloc(data_size));

        auto result = testLZ4(data, compressed_data, decompressed_data, data_size);
        speed << result.milliseconds << ',';
        compression << result.compression_ratio << ',';

        result = testZSTD1(data, compressed_data, decompressed_data, data_size);
        speed << result.milliseconds << ',';
        compression << result.compression_ratio << ',';

        result = testZSTD7(data, compressed_data, decompressed_data, data_size);
        speed << result.milliseconds << '\n';
        compression << result.compression_ratio << '\n';

        free(decompressed_data);
        free(compressed_data);
        free(data);
    }

    return 0;
}
