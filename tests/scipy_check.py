import sys
import time
import numpy as np
from scipy.io import mmread

path = sys.argv[1]

A = mmread(path).tocsr()  # load the .mtx and convert to CSR
x = np.ones(A.shape[1])   # same x as benchmark ( all ones )

# warm-up (not timed)
for _ in range(5):
    A @ x

times = []
for i in range(50):
    s = time.perf_counter()
    y=A@x
    e = time.perf_counter()
    times.append((e - s) * 1000) # milliseconds

median = np.median(times)
print(f"SciPy CSR median: {median:.3f} ms")
