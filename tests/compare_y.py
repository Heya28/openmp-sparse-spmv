import sys
import numpy as np
from scipy.io import mmread

mtx_path, y_path = sys.argv[1], sys.argv[2]

A = mmread(mtx_path).tocsr()
n = A.shape[1]
x = np.arange(n) % 10 + 1.0  # same x as dump_y.cpp: 1.0 + (i % 10)

y_scipy = A @ x
y_mine = np.loadtxt(y_path)

if np.allclose(y_scipy, y_mine, rtol=1e-10, atol=1e-9):
    print("SciPy correctness test successful. spmv_serial and scipy multiplications match!")
else:
    print("SciPy correctness test unsuccessful. spmv_serial and scipy multiplications do not match!")

print(f"Biggest difference: {np.max(np.abs(y_scipy - y_mine))}")