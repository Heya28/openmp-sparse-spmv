#include "../include/spmv.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept> 
#include <sstream>
#include<algorithm>
#include<cctype>
#include<omp.h>

COOMatrix read_mtx(const std::string& filename) {
    COOMatrix coo_matrix;
    std::ifstream mtxfile(filename);
    if (!mtxfile.is_open()) {
        throw std::runtime_error("Error: Could not open MTX file: " + filename);
    }

    std::string line;
    bool is_symmetric = false, is_skew = false, is_pattern = false;

    // Banner line: lowercase it, then check the format keywords
    if (std::getline(mtxfile, line)) {
        std::transform(line.begin(), line.end(), line.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        is_skew      = line.find("skew-symmetric") != std::string::npos;
        is_symmetric = !is_skew && line.find("symmetric") != std::string::npos;
        is_pattern   = line.find("pattern") != std::string::npos;
    }

    // Skip comments, then read the size line
    bool found_size = false;
    while (std::getline(mtxfile, line)) {
        if (line.empty() || line[0] == '%') continue;
        found_size = true;
        break;
    }
    if (!found_size) throw std::runtime_error("Error: MTX file has no size line: " + filename);

    std::stringstream ss(line);
    long long file_nnz = 0;
    ss >> coo_matrix.num_rows >> coo_matrix.num_cols >> file_nnz;

    const bool mirror = is_symmetric || is_skew;
    std::vector<int> coo_rows, coo_cols;
    std::vector<double> coo_values;
    size_t cap = (size_t)file_nnz * (mirror ? 2 : 1);
    coo_rows.reserve(cap);
    coo_cols.reserve(cap);
    coo_values.reserve(cap);

    while (std::getline(mtxfile, line)) {
        if (line.empty() || line[0] == '%') continue;
        std::stringstream ss_coo(line);
        int row, col;
        double value = 1.0;  // default for pattern matrices

        if (is_pattern) { if (!(ss_coo >> row >> col)) continue; }
        else            { if (!(ss_coo >> row >> col >> value)) continue; }

        row--; col--;  // 1-based -> 0-based
        coo_rows.push_back(row);
        coo_cols.push_back(col);
        coo_values.push_back(value);

        // Only one triangle is stored, so mirror off-diagonal entries
        if (mirror && row != col) {
            coo_rows.push_back(col);
            coo_cols.push_back(row);
            coo_values.push_back(is_skew ? -value : value);
        }
    }
    mtxfile.close();

    coo_matrix.total_nnz = (int)coo_rows.size();
    coo_matrix.rows   = std::move(coo_rows);
    coo_matrix.cols   = std::move(coo_cols);
    coo_matrix.values = std::move(coo_values);
    return coo_matrix;
}
CSRMatrix coo_to_csr(const COOMatrix& coo_matrix){
    int total_nnz=coo_matrix.total_nnz;
    std::vector<int> coo_rows=coo_matrix.rows;
    std::vector<int> coo_cols=coo_matrix.cols;
    std::vector<double> coo_values=coo_matrix.values;

    CSRMatrix matrix;
    matrix.num_rows = coo_matrix.num_rows;
    matrix.num_cols = coo_matrix.num_cols;
    // Converting COO to CSR (Histogram & Prefix Sum) // O(N)
    matrix.values.resize(total_nnz);
    matrix.col_indices.resize(total_nnz);
    matrix.row_ptr.assign(matrix.num_rows+1,0);

    // 1. Filling row_ptr 
    for(size_t i=0;i<coo_rows.size();i++){
        // count number of non-zeroes per row ( histogram )
        matrix.row_ptr[coo_rows[i]+1]++;
    }
    // prefix sum 
    for(int i=0;i<matrix.num_rows;i++){
        matrix.row_ptr[i+1]+=matrix.row_ptr[i];
    }

    // 2. Filling values and col_indices
    std::vector<int> current_dest = matrix.row_ptr; // temporary copy of row_ptr
    for (int i = 0; i < total_nnz; i++) {
        int r = coo_rows[i];
        int dest_index = current_dest[r]; // index at which current element should go in values and col_indices
        
        matrix.col_indices[dest_index] = coo_cols[i];
        matrix.values[dest_index] = coo_values[i];
        
        // Increment the destination index for the next time this row is seen
        current_dest[r]++;
        // This approach does not ensure that data is read left to right. It just groups elements by row.
        // But since spmv is dot multiplication, it does not matter.
    }
    return matrix;
}

// serial sparse matrix vector multiplication
// O(nnz + rows)
std::vector<double> spmv_serial(const CSRMatrix& matrix, const std::vector<double>& x) {
    std::vector<double> y(matrix.num_rows,0.0);
    // Matrix multiplication
    for(int i=0;i<matrix.num_rows;i++){
        double rowsum=0.0;
        for(int j=matrix.row_ptr[i];j<matrix.row_ptr[i+1];j++){
            rowsum+=x[matrix.col_indices[j]]*matrix.values[j];
        }
        y[i]=rowsum;
    }
    return y;
}
std::vector<double> spmv_balanced_serial(const CSRMatrix& matrix, const std::vector<double>& x, int T) {
    // * checks that multiplication is correct when rows are split at cut points ( no threads ) - test_balanced
    std::vector<double> y(matrix.num_rows, 0.0);
    long long total = matrix.row_ptr[matrix.num_rows];

    // 1. bound array built
    std::vector<int> bound(T + 1);
    for (int t = 0; t <= T; t++) {
        long long target = t * total / T; // how many non zeroes should come before this
        bound[t] = std::lower_bound(matrix.row_ptr.begin(), matrix.row_ptr.end(), target) - matrix.row_ptr.begin();
    }

    // 2. for each worker t, loop over its rows
    for (int t = 0; t < T; t++) {
        for (int i = bound[t]; i < bound[t + 1]; i++) {
            double rowsum=0.0;
            for(int j=matrix.row_ptr[i];j<matrix.row_ptr[i+1];j++){
                rowsum+=x[matrix.col_indices[j]]*matrix.values[j];
            }
            y[i]=rowsum;
        }
    }
    return y;
}
// Parallel sparse matrix vector multiplication
std::vector<double> spmv_omp(const CSRMatrix& matrix, const std::vector<double>& x){
    std::vector<double> y(matrix.num_rows,0.0);
    #pragma omp parallel for schedule(static)
    for(int i=0;i<matrix.num_rows;i++){
        double rowsum=0.0;
        for(int j=matrix.row_ptr[i];j<matrix.row_ptr[i+1];j++){
            rowsum+=x[matrix.col_indices[j]]*matrix.values[j];
        }
        y[i]=rowsum;
    }
    return y;
}

std::vector<double> spmv_omp_balanced(const CSRMatrix& matrix, const std::vector<double>& x) {
    std::vector<double> y(matrix.num_rows, 0.0); // created before the parallel region, so all threads share it.
    int T = omp_get_max_threads(); // How many threads OpenMP would use by default

    std::vector<int> bound(T + 1);
    long long total = matrix.row_ptr[matrix.num_rows];
    for (int t = 0; t <= T; t++) {
        long long target = t * total / T; // how many non zeroes should come before this
        bound[t] = std::lower_bound(matrix.row_ptr.begin(), matrix.row_ptr.end(), target) - matrix.row_ptr.begin();
    }

    #pragma omp parallel num_threads(T)
    {
        int t = omp_get_thread_num();
        for (int i = bound[t]; i < bound[t + 1]; i++) {
            double rowsum=0.0;
            for(int j=matrix.row_ptr[i];j<matrix.row_ptr[i+1];j++){
                rowsum+=x[matrix.col_indices[j]]*matrix.values[j];
            }
            y[i]=rowsum;
        }
    }
    return y;
}
