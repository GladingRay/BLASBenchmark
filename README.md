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