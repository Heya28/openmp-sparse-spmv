#include "../include/spmv.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

int main(int argc, char** argv) {
    CSRMatrix csr = coo_to_csr(read_mtx(argv[1]));
    std::vector<double> x(csr.num_cols);
    for (int i = 0; i < csr.num_cols; i++) x[i] = 1.0 + (i % 10);

    auto y_ref = spmv_serial(csr, x);
    auto y_bs = spmv_balanced_serial(csr, x, 4);
    auto y_ob  = spmv_omp_balanced(csr, x);

    double e1 = 0, e2 = 0;
    for (size_t i = 0; i < y_ref.size(); i++) {
        e1 = std::max(e1, std::abs(y_ref[i] - y_bs[i]));
        e2 = std::max(e2, std::abs(y_ref[i] - y_ob[i]));
    }
    std::cout << "balanced_serial err = " << e1 << ", omp_balanced err = " << e2 << "\n";
}