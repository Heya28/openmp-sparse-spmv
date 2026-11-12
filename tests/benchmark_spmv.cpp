#include "../include/spmv.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cmath>

// Helper to run warmups and calculate the median time of 50 runs
template <class F>
double median_ms(F f, int warmup = 5, int iters = 50) {
    for (int i = 0; i < warmup; i++) f();
    std::vector<double> t(iters);
    for (int i = 0; i < iters; i++) {
        auto s = std::chrono::steady_clock::now();
        f();
        auto e = std::chrono::steady_clock::now();
        t[i] = std::chrono::duration<double, std::milli>(e - s).count();
    }
    std::sort(t.begin(), t.end());
    return t[iters / 2];
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./benchmark_spmv <path_to_matrix.mtx>" << std::endl;
        return 1;
    }

    std::cout << "Loading matrix: " << argv[1] << "..." << std::endl;
    COOMatrix coo = read_mtx(argv[1]);
    CSRMatrix csr = coo_to_csr(coo);
    
    std::cout << "Matrix loaded! Rows: " << csr.num_rows << ", Non-zeros: " << csr.values.size() << "\n" << std::endl;

    std::vector<double> x(csr.num_cols, 1.0);

    // Correctness Check (Tolerance of 1e-9 for floating point drift)
    std::vector<double> y_serial = spmv_serial(csr, x);
    std::vector<double> y_omp = spmv_omp(csr, x);
    for (size_t i = 0; i < y_serial.size(); i++) {
        if (std::abs(y_serial[i] - y_omp[i]) > 1e-9) {
            std::cerr << "FATAL: OpenMP output does not match Serial output at index " << i << "!" << std::endl;
            return 1;
        }
    }
    std::cout << "Correctness Verified: OpenMP matches Serial.\n" << std::endl;

    std::vector<double> y_bal = spmv_omp_balanced(csr, x);
    for (size_t i = 0; i < y_serial.size(); i++) {
        if (std::abs(y_serial[i] - y_bal[i]) > 1e-9) {
            std::cerr << "FATAL: Balanced output does not match Serial at index " << i << "!" << std::endl;
            return 1;
        }
    }
    std::cout << "Correctness Verified: Balanced matches Serial.\n" << std::endl;
    
    //  Stable Benchmarking
    double sink = 0;
    double serial_ms = median_ms([&]{ auto y = spmv_serial(csr, x); sink += y[0]; });
    double omp_ms    = median_ms([&]{ auto y = spmv_omp(csr, x);    sink += y[0]; });
    double bal_ms    = median_ms([&]{ auto y = spmv_omp_balanced(csr, x);    sink += y[0]; });

    std::cout << "Serial Time (Median): " << serial_ms << " ms" << std::endl;
    std::cout << "OpenMP Time (Median): " << omp_ms << " ms" << std::endl;
    std::cout << "Balanced OpenMP Time (Median): " << bal_ms << " ms" << std::endl;
    std::cout << "Speedup (OpenMP vs Serial): " << serial_ms / omp_ms << "x" << std::endl;
    std::cout << "Speedup (Balanced vs OpenMP): " << omp_ms / bal_ms<< "x" << std::endl;
    std::cout << "Speedup (Balanced vs Serial): " << serial_ms / bal_ms << "x" << std::endl;
    std::cout << "(Checksum: " << sink << ")\n" << std::endl;

    return 0;
}