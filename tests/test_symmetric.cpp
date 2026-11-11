#include "../include/spmv.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    COOMatrix coo = read_mtx("tests/sym3.mtx");
    assert(coo.total_nnz == 5);  // (2,1) was mirrored to (1,2)
    CSRMatrix csr = coo_to_csr(coo);
    std::vector<double> y = spmv_serial(csr, {1.0, 1.0, 1.0});
    assert(std::abs(y[0] - 5.0) < 1e-12);
    assert(std::abs(y[1] - 7.0) < 1e-12);
    assert(std::abs(y[2] - 5.0) < 1e-12);
    std::cout << "SUCCESS: symmetric parsing works\n";
}