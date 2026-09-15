#include<iostream>
#include"spmv.hpp"

int main(){
        std::vector<std::vector<double>> A = {
        {10, 0, 0, 20},
        {0, 30, 0, 0},
        {40, 0, 50, 0}
    };
    std::vector<double> values;
    std::vector<int> column_indices;
    std::vector<int> row_ptr;
    convertToCSR(A,values,column_indices,row_ptr);
    std::vector<double> y;
    std::vector<double> x={1,2,3,4}; // x vector (4x1)
    spmv(values,column_indices,row_ptr,x,y);
    for(double value:y){
        std::cout<<value<<" ";
    }
    std::cout<<"\n";
    return 0;
}