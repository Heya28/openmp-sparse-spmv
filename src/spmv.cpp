#include "spmv.hpp"
void convertToCSR( const std::vector<std::vector<double>>& A, std::vector<double>& values, std::vector<int>& column_indices, std::vector<int>& row_ptr){
    int num_rows=A.size();
    int num_cols=A[0].size();
    row_ptr.push_back(0);
    for(int i=0;i<num_rows;i++){
        for(int j=0;j<num_cols;j++){
            if(A[i][j]!=0){
                values.push_back(A[i][j]);
                column_indices.push_back(j);
            }
        }
        row_ptr.push_back(values.size());
    }
}
void spmv( const std::vector<double>& values, const std::vector<int>& column_indices, const std::vector<int>& row_ptr, const std::vector<double>& x,
std::vector<double>& y){
    int num_rows=row_ptr.size()-1;
    y.assign(num_rows,0);
    for(int i=0;i<num_rows;i++){
        for(int k=row_ptr[i]; k<row_ptr[i+1];k++){
            y[i]+=values[k]*x[column_indices[k]];
        }
    }
}