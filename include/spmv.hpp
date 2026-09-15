#pragma once
#include<vector>
void convertToCSR(const std::vector<std::vector<double>>& A, std::vector<double>& values, std::vector<int>& column_indices, std::vector<int>& row_ptr);

void spmv(const std::vector<double>& values, const std::vector<int>& column_indices, const std::vector<int>& row_ptr, const std::vector<double>& x,
std::vector<double>& y);