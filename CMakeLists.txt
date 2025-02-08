cmake_minimum_required(VERSION 3.1)
project(BLASBenchmark VERSION 1.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Release)
set(CXX_STANDARD_REQUIRED 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

set(BLISINC_PATH ${CMAKE_SOURCE_DIR}/BLASINSTALL/blis/include/)
set(BLISLIB ${CMAKE_SOURCE_DIR}/BLASINSTALL/blis/lib/libblis.a)
set(OpenBLASINC_PATH ${CMAKE_SOURCE_DIR}/BLASINSTALL/OpenBLAS/include/)
set(OpenBLASLIB ${CMAKE_SOURCE_DIR}/BLASINSTALL/OpenBLAS/lib/libopenblas.a)

add_executable(benchmark_OpenBLAS src/benchmark.cc)
target_link_libraries(benchmark_OpenBLAS PRIVATE ${OpenBLASLIB})
target_include_directories(benchmark_OpenBLAS PRIVATE ${OpenBLASINC_PATH})
target_compile_definitions(benchmark_OpenBLAS PRIVATE USE_OPENBLAS)

add_executable(benchmark_blis src/benchmark.cc)
target_link_libraries(benchmark_blis PRIVATE ${BLISLIB})
target_include_directories(benchmark_blis PRIVATE ${BLISINC_PATH})
target_compile_definitions(benchmark_blis PRIVATE USE_BLIS)