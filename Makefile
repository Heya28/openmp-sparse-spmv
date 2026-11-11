CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17 -fopenmp

all: test_spmv benchmark_spmv

test_spmv: tests/test_spmv.cpp src/spmv.cpp
	$(CXX) $(CXXFLAGS) -o test_spmv tests/test_spmv.cpp src/spmv.cpp

benchmark_spmv: tests/benchmark_spmv.cpp src/spmv.cpp
	$(CXX) $(CXXFLAGS) -o benchmark_spmv tests/benchmark_spmv.cpp src/spmv.cpp

clean:
	rm -f test_spmv benchmark_spmv