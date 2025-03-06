# BLASBenchmark
Benchmark for some open source BLAS libs.
## How to clone
```bash
git clone --recurse-submodules https://github.com/GladingRay/BLASBenchmark.git
```
## How to build
```bash
mkdir build
cd build
cmake ..
make benchmark_OpenBLAS
make benchmark_blis
make cpufp_bin # build cpufp
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
## OpenBLAS调试记录
`OPENBLAS_NUM_THREADS=1`
1. interface/gemm.c:295
2. interface/gemm.c:620
3. driver/level3.c:172
4. driver/level3.c:369
5. kernel/x86_64/dgemm_kernel_16x2_skylakex.c:560
```c
for(;n_count>11;n_count-=12) COMPUTE(12)
for(;n_count>9;n_count-=10) COMPUTE(10)
for(;n_count>7;n_count-=8) COMPUTE(8)
for(;n_count>5;n_count-=6) COMPUTE(6)
for(;n_count>3;n_count-=4) COMPUTE(4)
for(;n_count>1;n_count-=2) COMPUTE(2)
if(n_count>0) COMPUTE(1)
```
从这里看B矩阵在N方向上的切分方式是23->(12+10+1)，而不是(12+11)，B矩阵的打包也采用一样的划分方式，不太好生成。

## Reference & Acknowledge
- [cpufp](https://github.com/pigirons/cpufp)
- [OpenBLAS](https://github.com/OpenMathLib/OpenBLAS)
- [blis](https://github.com/flame/blis)
- 感谢AI