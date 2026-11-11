# OpenMP Sparse Matrix-Vector Multiplication (SpMV)

Serial and OpenMP-parallel SpMV on CSR matrices in C++17, with a benchmark
harness to study how performance scales with thread count.

## Status
- [x] Matrix Market (.mtx) parser -> COO -> CSR conversion
- [x] Serial SpMV, verified against a dense reference on a 4x4 matrix
- [x] Naive OpenMP version (`schedule(static)`)
- [x] Benchmark harness (warm-up + median timing)
- [x] Synthetic matrix generator (uniform)
- [ ] Symmetric / pattern .mtx support in the parser
- [ ] Banded, near-diagonal and power-law test matrices
- [ ] Real SuiteSparse matrices (host currently returns 403)
- [ ] Dynamic/guided scheduling comparison
- [ ] Speedup plots

## Build and run
```bash
make
./test_spmv                      # correctness check
./generate_matrix                # writes data/massive.mtx (synthetic)
OMP_NUM_THREADS=4 OMP_PROC_BIND=true ./benchmark_spmv data/massive.mtx
```
Requires g++ with OpenMP (`sudo apt install build-essential`).

## Test machine
Intel i5-13420H (6 cores / 12 threads), 12 MiB L3, WSL2 Ubuntu 24.04.

## Preliminary results
Matrix: synthetic uniform random, 100,000 x 100,000, 5M non-zeros (~60 MB, larger than L3).

| Threads | OpenMP time (ms) |
|---|---|
| 1 | 11.08 |
| 2 | 6.88 |
| 4 | 3.25 |
| 6 | 3.78 |
| 8 | 3.86 |
| 12 | 6.17 |

Scaling is good up to 4 threads, flat at 6-8, and worse at 12 (hyperthreads
share cores). Memory bandwidth saturation is the likely cause, but this is
not yet confirmed.

**Caveats:** results are from a single sweep, the serial baseline varied
between 7.6 and 11.3 ms across runs, and WSL2 adds noise. Numbers will be
redone with a fixed baseline and repeated runs.

## Known limitations
- The parser only handles `general` real matrices.
- Only a synthetic matrix has been tested so far.