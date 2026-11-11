#pragma once
#include <vector>
#include <string>

// Temporary structure for holding raw file data
struct COOMatrix {
    int num_rows;
    int num_cols;
    int total_nnz;
    std::vector<int> rows;
    std::vector<int> cols;
    std::vector<double> values;
};

// CSR struct 
struct CSRMatrix {
    int num_rows;
    int num_cols;
    std::vector<double> values;
    std::vector<int> col_indices;
    std::vector<int> row_ptr;
};

COOMatrix read_mtx(const std::string& filename);
CSRMatrix coo_to_csr(const COOMatrix& coo_matrix);
std::vector<double> spmv_serial(const CSRMatrix& matrix, const std::vector<double>& x);
std::vector<double> spmv_omp(const CSRMatrix& matrix, const std::vector<double>& x);