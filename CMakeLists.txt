cmake_minimum_required(VERSION 3.10)

project(hd-sketch)
project(ninja LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-Ofast -march=native -mtune=native")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(dot-test src/HDTest/dot-test.cc)
add_executable(benchmark 
    src/benchmarks/benchmark.cc
    src/utils/fasta.cc 
    src/utils/MurmurHash.cc 
    src/utils/utils.cc
    )