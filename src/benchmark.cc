#include "cblas.h"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>

using ClockType = std::chrono::high_resolution_clock;
using TimePointType = std::chrono::time_point<ClockType>;
using DuraType = std::chrono::duration<double, std::milli>;

volatile void flushAllCache()
{
    // TODO: Get the actual cache size based on the specific platform
    // 32M bytes = 2^25 bytes = 2^22 double
    constexpr size_t CacheSizeInDouble = 1 << 22;
    volatile double *cache = new double[CacheSizeInDouble];
    volatile double sum = 0;
    for (int i = 0; i < CacheSizeInDouble; i++)
    {
        sum += cache[i];
    }
    delete[] cache;
}

void initMatrix(double *matrix, int m, int n, int lda)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-10, 10);
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < m; i++)
        {
            matrix[i + j * lda] = dis(gen);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        printf("Usage: %s trans_lhs trans_rhs m n k\n", argv[0]);
        exit(1);
    }

    CBLAS_TRANSPOSE trans_lhs = argv[1][0] == 'N' ? CblasNoTrans : CblasTrans;
    CBLAS_TRANSPOSE trans_rhs = argv[2][0] == 'N' ? CblasNoTrans : CblasTrans;

    int m = std::atoi(argv[3]);
    int n = std::atoi(argv[4]);
    int k = std::atoi(argv[5]);

    int lda = m;
    int ldb = k;
    int ldc = m;

    double ops = 2.0 * m * n * k;

    constexpr size_t AlignSizeInByte = 128;

    double *lhs = static_cast<double *>(
        aligned_alloc(AlignSizeInByte, m * k * sizeof(double)));
    double *rhs = static_cast<double *>(
        aligned_alloc(AlignSizeInByte, n * k * sizeof(double)));
    double *c = static_cast<double *>(
        aligned_alloc(AlignSizeInByte, m * n * sizeof(double)));

    initMatrix(lhs, m, k, lda);
    initMatrix(rhs, k, n, ldb);
    initMatrix(c, m, n, ldc);

    double alpha = 3.141;
    double beta = 1.732;

    flushAllCache();

    TimePointType start = ClockType::now();
    cblas_dgemm(CblasColMajor, trans_lhs, trans_rhs, m, n, k, alpha, lhs, lda,
        rhs, ldb, beta, c, ldc);
    TimePointType end = ClockType::now();
    DuraType dura = end - start;
    double ms = dura.count();
    double gflops = (ops / 1e6) / ms;

    printf(
        "blasname:%s, shape:%d*%d*%d, ops:%.2f, time(ms):%.4f, gflops:%.4f\n",
#ifdef USE_OPENBLAS
        "OpenBLAS"
#undef USE_OPENBLAS
#elif defined(USE_BLIS)
        "blis"
#undef USE_BLIS
#else
        "unknown"
#endif
        ,
        m, n, k, ops, ms, gflops);

    free(lhs);
    free(rhs);
    free(c);

    return 0;
}