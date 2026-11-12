#include "../include/spmv.hpp"
#include <iostream>
#include<algorithm>
#include<vector>
#include<string>

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
    
    std::vector<int> bound(T + 1);
    for(int t=0;t<=T;t++){
        long long target = t * total / T; // how many non zeroes should come before this
        bound[t] = std::lower_bound(m.row_ptr.begin(), m.row_ptr.end(), target) - m.row_ptr.begin(); // searches the sorted row_ptr array for the first entry that is >= target.
        // - m.row_ptr.begin() to convert iterator return type to an index 
    }
    long long mx2 = 0;
    for (int t = 0; t < T; t++) {
        int lo=bound[t], hi=bound[t+1];
        long long work = m.row_ptr[hi]-m.row_ptr[lo];
        mx2 = std::max(mx2, work);
    }
    std::cout << "T=" << T << "  equal rows  = " << (double)mx / ((double)total / T) << "  equal nnz = " << (double)mx2 / ((double)total / T) << "\n";
}