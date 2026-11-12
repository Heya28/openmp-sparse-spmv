#include <omp.h>
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    const size_t N = 25'000'000;   // 25M doubles = 200 MB per array, much bigger than the 12 MiB cache
    std::vector<double> a(N), b(N);

    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < N; i++) {
        a[i] = 1.0; b[i] = 0.0;
    }

    // Test 1: copy (reads a, writes b)
    std::vector<double> times;
    for (int rep = 0; rep < 12; rep++) {
        double s = omp_get_wtime();
        #pragma omp parallel for schedule(static)
        for (size_t i = 0; i < N; i++) {
            b[i] = a[i];
        }
        double e = omp_get_wtime();
        if (rep >= 2) times.push_back(e - s);   // skip 2 warm-up reps
    }
    std::sort(times.begin(), times.end());
    double med = times[times.size() / 2];
    double gbps = 2.0 * N * 8 / med / 1e9;   // read 8 + write 8 bytes per element
    std::cout << "threads=" << omp_get_max_threads() << "  copy bandwidth = " << gbps << " GB/s\n";

    // Test 2: read-only (sums a) 
    double sum = 0;
    std::vector<double> rtimes;
    for (int rep = 0; rep < 12; rep++) {
        sum = 0;
        double s = omp_get_wtime();
        #pragma omp parallel for schedule(static) reduction(+:sum)
        for (size_t i = 0; i < N; i++) sum += a[i];
        double e = omp_get_wtime();
        if (rep >= 2) rtimes.push_back(e - s);
    }
    std::sort(rtimes.begin(), rtimes.end());
    double rmed = rtimes[rtimes.size() / 2];
    double rgbps = (double)N * 8 / rmed / 1e9;  // only reads: 8 bytes per element
    std::cout << "threads=" << omp_get_max_threads() << "  read bandwidth = " << rgbps
              << " GB/s  (sum=" << sum << ")\n";

    std::cout << b[N / 2] << "\n";   // use b so the copy can't be removed
}