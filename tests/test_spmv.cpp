#include "../include/spmv.hpp"
#include <iostream>
#include <vector>
#include <cassert>

// Brute-force dense SpMV strictly for correctness testing
std::vector<double> dense_spmv(const std::vector<double>& dense_matrix, int num_rows, int num_cols, const std::vector<double>& x) {
    std::vector<double> y(num_rows, 0.0);

    // Dense matrix multiplication
    for(int i=0;i<num_rows;i++){
        double rowsum=0;
        for(int j=0;j<num_cols;j++){
            rowsum+=dense_matrix[i*num_cols+j]*x[j];
        }
        y[i]=rowsum;
    }
    return y;
}

int main() {
    // tiny 4x4 coo matrix 
    COOMatrix coo_matrix;
    coo_matrix.num_rows=4, coo_matrix.num_cols=4, coo_matrix.total_nnz = 5;
    coo_matrix.rows={0,1,1,2,3}, coo_matrix.cols={0,1,3,2,1}, coo_matrix.values={5.0,8.0,2.0,3.0,6.0};
    
    std::vector<double> dense_matrix = {
        5.0, 0.0, 0.0, 0.0,
        0.0, 8.0, 0.0, 2.0,
        0.0, 0.0, 3.0, 0.0,
        0.0, 6.0, 0.0, 0.0
    };
    std::vector<double> x={1.0,1.0,1.0,1.0};
    // Convert coo to csr
    CSRMatrix csr_matrix;
    csr_matrix=coo_to_csr(coo_matrix);

    std::vector<double> y_csr=spmv_serial(csr_matrix,x);
    std::vector<double> y_dense=dense_spmv(dense_matrix, 4, 4, x);
    for (int i = 0; i < 4; i++) {
        assert(y_csr[i] == y_dense[i]); 
    }

    std::cout << "SUCCESS: CSR SpMV matches Dense SpMV exactly!" << std::endl;  
    return 0;
}