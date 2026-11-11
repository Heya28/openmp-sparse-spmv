#include <iostream>
#include <fstream>
#include <random>

int main() {
    int rows = 100000;
    int cols = 100000;
    int nnz = 5000000; // 5 million elements

    std::cout << "Generating massive.mtx" << std::endl;
    std::ofstream file("data/massive.mtx");
    
    // Standard MTX Header
    file << "%%MatrixMarket matrix coordinate real general\n";
    file << "% Synthetic stress test matrix\n";
    file << rows << " " << cols << " " << nnz << "\n";

    std::default_random_engine generator(42); // Seeded for reproducibility
    std::uniform_int_distribution<int> row_dist(1, rows);
    std::uniform_int_distribution<int> col_dist(1, cols);
    std::uniform_real_distribution<double> val_dist(-10.0, 10.0);

    // Generate random coordinates and values
    for (int i = 0; i < nnz; ++i) {
        file << row_dist(generator) << " " << col_dist(generator) << " " << val_dist(generator) << "\n";
    }

    file.close();
    std::cout << "Success! 5 million non-zeros written to data/massive.mtx" << std::endl;
    return 0;
}