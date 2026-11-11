#include "../include/spmv.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept> 
#include <sstream>

COOMatrix read_mtx(const std::string& filename) {
    COOMatrix coo_matrix;
    std::ifstream mtxfile(filename);
    if (!mtxfile.is_open()) {
        throw std::runtime_error("Error: Could not open MTX file: " + filename);
    }
    
    std::string line;
    while (std::getline(mtxfile, line)) {
        if (line.empty() || line[0] == '%') {
            continue;
        } else {
            break;
        }
    } 
    // Parse the dimension header 
    std::stringstream ss(line);
    int total_nnz;
    ss >> coo_matrix.num_rows >> coo_matrix.num_cols >> total_nnz;
    
    // Parse the rest of the file
    std::vector<int> coo_rows;
    std::vector<int> coo_cols;
    std::vector<double> coo_values;
    coo_rows.reserve(total_nnz);
    coo_cols.reserve(total_nnz);
    coo_values.reserve(total_nnz);
    
    while (std::getline(mtxfile, line)) {
        std::stringstream ss_coo(line);
        int row, col;
        double value;
        ss_coo >> row >> col >> value;
        
        row--; // Convert 1-based index to 0-based index
        col--; 
        
        coo_rows.push_back(row);
        coo_cols.push_back(col);
        coo_values.push_back(value);
    }
    mtxfile.close();
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
    for(int i=0;i<coo_rows.size();i++){
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
