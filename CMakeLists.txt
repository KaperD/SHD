cmake_minimum_required(VERSION 3.10)
project(SHD)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        main main.cpp
)

target_compile_options(main PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(main ${CMAKE_SOURCE_DIR}/../lib/liblz4.a)
target_link_libraries(main ${CMAKE_SOURCE_DIR}/../lib/libzstd.a)