cmake_minimum_required(VERSION 3.5)
project(BLASBenchmark VERSION 1.0 LANGUAGES CXX)

set(CXX_STANDARD_REQUIRED 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

include(${CMAKE_CURRENT_BINARY_DIR}/config.cmake)

include(ExternalProject)

set(OpenBLAS_DEBUG 0)
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(OpenBLAS_DEBUG 1)
endif()

ExternalProject_Add(
    OpenBLASLib
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/BLASLIBS/OpenBLAS
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make -j DEBUG=${OpenBLAS_DEBUG}
    INSTALL_COMMAND make PREFIX=../../BLASINSTALL/OpenBLAS/ install
    BUILD_IN_SOURCE TRUE
)

set(BLIS_DEBUG "")
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(BLIS_DEBUG "--enable-debug")
endif()

ExternalProject_Add(
    BLISLib
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/BLASLIBS/blis
    CONFIGURE_COMMAND ./configure -p ../../BLASINSTALL/blis ${BLIS_DEBUG} --enable-static --enable-shared -t openmp --enable-blas --enable-cblas auto
    BUILD_COMMAND make -j
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE TRUE
)

ExternalProject_Add(
    cpufp_bin
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_tools/cpufp
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ./build_x64.sh
    INSTALL_COMMAND mv ./cpufp ${CMAKE_CURRENT_BINARY_DIR}
    BUILD_IN_SOURCE TRUE
)

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

add_dependencies(benchmark_OpenBLAS OpenBLASLib)
add_dependencies(benchmark_blis BLISLib)