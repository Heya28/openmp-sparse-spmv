import gzip, sys

src, dst = sys.argv[1], sys.argv[2]
transpose = len(sys.argv) > 3 and sys.argv[3] == "transpose"
opener = gzip.open if src.endswith(".gz") else open

def edges():
    with opener(src, "rt") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            u, v = map(int, line.split()[:2])
            yield (v, u) if transpose else (u, v)

n = m = 0
for u, v in edges():
    n = max(n, u + 1, v + 1)
    m += 1

with open(dst, "w") as out:
    out.write("%%MatrixMarket matrix coordinate pattern general\n")
    out.write(f"{n} {n} {m}\n")
    for u, v in edges():
        out.write(f"{u + 1} {v + 1}\n")
print(f"{n} x {n}, {m} non-zeros -> {dst}")