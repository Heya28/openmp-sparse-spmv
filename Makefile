CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17 -fopenmp

all: test_spmv test_symmetric benchmark_spmv imbalance generate_matrix test_balanced dump_y stream_copy

test_spmv: tests/test_spmv.cpp src/spmv.cpp include/spmv.hpp
	$(CXX) $(CXXFLAGS) -o test_spmv tests/test_spmv.cpp src/spmv.cpp

test_symmetric: tests/test_symmetric.cpp src/spmv.cpp include/spmv.hpp
	$(CXX) $(CXXFLAGS) -o test_symmetric tests/test_symmetric.cpp src/spmv.cpp

benchmark_spmv: tests/benchmark_spmv.cpp src/spmv.cpp include/spmv.hpp
	$(CXX) $(CXXFLAGS) -o benchmark_spmv tests/benchmark_spmv.cpp src/spmv.cpp

imbalance: tests/imbalance.cpp src/spmv.cpp include/spmv.hpp
	$(CXX) $(CXXFLAGS) -o imbalance tests/imbalance.cpp src/spmv.cpp

generate_matrix: generate_matrix.cpp
	$(CXX) $(CXXFLAGS) -o generate_matrix generate_matrix.cpp

test_balanced: tests/test_balanced.cpp src/spmv.cpp include/spmv.hpp
	$(CXX) $(CXXFLAGS) -o test_balanced tests/test_balanced.cpp src/spmv.cpp

dump_y: tests/dump_y.cpp src/spmv.cpp include/spmv.hpp
	$(CXX) $(CXXFLAGS) -o dump_y tests/dump_y.cpp src/spmv.cpp

stream_copy: tests/stream_copy.cpp
	$(CXX) $(CXXFLAGS) -o stream_copy tests/stream_copy.cpp

clean:
	rm -f test_spmv test_symmetric benchmark_spmv imbalance generate_matrix test_balanced dump_y stream_copy

.PHONY: all clean