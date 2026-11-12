#include "../include/spmv.hpp"
#include <iostream>
#include<algorithm>

int main(int argc, char** argv) {
    CSRMatrix m = coo_to_csr(read_mtx(argv[1]));
    int n = m.num_rows;
    int T = std::stoi(argv[2]);

    int chunk = (n+ T-1)/T;   // rows per thread
    long long total = m.row_ptr[n];
    long long mx = 0;
    for (int t = 0; t < T; t++) {
        int lo = t*chunk; // first row of block t
        int hi = std::min(n,(t+1)*chunk); // one past the last row of block t, capped at n
        long long work = m.row_ptr[hi]-m.row_ptr[lo] ; // non-zeros in rows lo up to hi */
        mx = std::max(mx, work);
    }
    std::cout << "T=" << T << "  max/ideal = " << (double)mx / ((double)total / T) << "\n";
}