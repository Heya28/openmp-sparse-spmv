/*write a tiny program,
 tests/dump_y.cpp, that loads a matrix, builds x[i] = 1.0 + (i % 10), 
 calls spmv_serial, and writes y to a text file, one number per line (std::ofstream with std::setprecision(17)). 
 Add it to the Makefile like your other programs.
 */

#include "../include/spmv.hpp"
#include <iostream>
#include<algorithm>
#include<vector>
#include <fstream>
#include<iomanip>

int main(int argc, char** argv){
    if (argc < 3) { 
        std::cerr << "Usage: ./dump_y <matrix.mtx> <out.txt>\n"; return 1; 
    }
    CSRMatrix m = coo_to_csr(read_mtx(argv[1]));
    std::vector<double> x;
    int num_cols=m.num_cols;
    x.reserve(num_cols);
    for(int i=0;i<num_cols;i++){
        x.push_back(1.0 + (i%10));
    }
    std::vector<double> y_serial=spmv_serial(m,x);
    std::ofstream out(argv[2]);
    out << std::setprecision(17);   // 17 digits so the doubles round-trip ( double - text - double ) exactly - With the default 6 digits, the two would differ by about 1e-6 even if both were correct
    for (size_t i = 0; i < y_serial.size(); i++) {
        out << y_serial[i] << "\n";
    }

}


