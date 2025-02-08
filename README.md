# BLASBenchmark
Benchmark for some open source BLAS lib
## How to build and install OpenBLAS
```bash
cd BLASLIBS/OpenBLAS/
make -j8
make PREFIX=../../BLASINSTALL/OpenBLAS/ install
```
## How to build and install blis
```bash
cd BLASLIBS/blis/
./configure -p ../../BLASINSTALL/blis --enable-static --enable-shared -t openmp --enable-blas --enable-cblas zen3
make -j8
make install
```
## How to build this benchmark
```bash
mkdir build
cd build
cmake ..
make benchmark*
```
## Multithreading
```bash
# thread to core affinity
export OMP_PROC_BIND=close
export OMP_PLACES=cores

# for OpenBLAS
export OPENBLAS_NUM_THREADS=8

# for blis
export BLIS_NUM_THREADS=8
```
## R7 5800H (zen3) peak flops
tool: [cpufp](https://github.com/pigirons/cpufp)
```bash
> ./cpufp --thread_pool=\[0\]
```
Number Threads: 1
Thread Pool Binding: 0

| Instruction Set | Core Computation      | Peak Performance |
|-----------------|-----------------------|------------------|
| FMA             | FMA(f32,f32,f32)      | 136.76 GFLOPS    |
| FMA             | FMA(f64,f64,f64)      | **67.115 GFLOPS**    |
| AVX             | ADD(MUL(f32,f32),f32) | 132.43 GFLOPS    |
| AVX             | ADD(MUL(f64,f64),f64) | 67.111 GFLOPS    |
| SSE             | ADD(MUL(f32,f32),f32) | 68.256 GFLOPS    |
| SSE2            | ADD(MUL(f64,f64),f64) | 32.769 GFLOPS    |

