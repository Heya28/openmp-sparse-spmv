#include<iostream>
#include<vector>
using namespace std;
void convertToCSR( const vector<vector<double>>& A, vector<double>& values, vector<int>& column_indices, vector<int>& row_ptr){
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
void spmv( const vector<double>& values, const vector<int>& column_indices, const vector<int>& row_ptr, const vector<double>& x,
vector<double>& y){
    int num_rows=row_ptr.size()-1;
    y.assign(num_rows,0);
    for(int i=0;i<num_rows;i++){
        for(int k=row_ptr[i]; k<row_ptr[i+1];k++){
            y[i]+=values[k]*x[column_indices[k]];
        }
    }
}
int main(){
        vector<vector<double>> A = {
        {10, 0, 0, 20},
        {0, 30, 0, 0},
        {40, 0, 50, 0}
    };
    vector<double> values;
    vector<int> column_indices;
    vector<int> row_ptr;
    convertToCSR(A,values,column_indices,row_ptr);
    vector<double> y;
    vector<double> x={1,2,3,4}; // x vector (4x1)
    spmv(values,column_indices,row_ptr,x,y);
    for(double value:y){
        cout<<value<<" ";
    }
    cout<<"\n";
    return 0;
}