# config options:
# CMAKE_BUILD_TYPE  : [Release/Debug]
# CPUFP_BUILD_ARCH  : [arm64/e2k/loogarch64/riscv64/x64]

cmake_minimum_required(VERSION 3.5)
project(BLASBenchmark VERSION 1.0 LANGUAGES CXX)

set(CXX_STANDARD_REQUIRED 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/config.cmake)
    message(STATUS "Build with config.cmake")
    include(${CMAKE_CURRENT_BINARY_DIR}/config.cmake)
else()
    message(WARNING "Build without config.cmake, please check cmdargs.")
endif()

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
    BUILD_COMMAND ./build_${CPUFP_BUILD_ARCH}.sh
    INSTALL_COMMAND mv ./cpufp ${CMAKE_CURRENT_BINARY_DIR} && sh clean.sh
    BUILD_IN_SOURCE TRUE
)

set(BLIS_INSTALL_PATH ${CMAKE_SOURCE_DIR}/BLASINSTALL/blis/)
set(OpenBLAS_INSTALL_PATH ${CMAKE_SOURCE_DIR}/BLASINSTALL/OpenBLAS/)
set(BLISINC_PATH ${BLIS_INSTALL_PATH}/include/)
set(BLISLIB ${BLIS_INSTALL_PATH}/lib/libblis.a)
set(OpenBLASINC_PATH ${OpenBLAS_INSTALL_PATH}/include/)
set(OpenBLASLIB ${OpenBLAS_INSTALL_PATH}/lib/libopenblas.a)

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

add_custom_target(uninstallblas
    COMMAND rm -rf ${OpenBLAS_INSTALL_PATH}/include ${OpenBLAS_INSTALL_PATH}/bin
                    ${OpenBLAS_INSTALL_PATH}/lib ${BLIS_INSTALL_PATH}/include
                    ${BLIS_INSTALL_PATH}/lib ${BLIS_INSTALL_PATH}/share ${CMAKE_BINARY_DIR}/cpufp
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/BLASLIBS/OpenBLAS clean
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/BLASLIBS/blis clean
    COMMENT "Uninstall OpenBLAS and blis."
)