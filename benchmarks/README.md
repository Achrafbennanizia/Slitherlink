# Benchmarks README

## Overview

This directory contains benchmarking tools for the Slitherlink solver to measure and analyze performance.

## Contents

1. **run_benchmarks.sh** - Shell script for quick benchmarking
2. **performance_benchmark.cpp** - Comprehensive C++ benchmark tool
3. **benchmark_results.txt** - Latest benchmark results (generated)
4. **benchmark_results.csv** - CSV export for analysis (generated)

## Usage

### Quick Benchmark (Shell Script)

```bash
cd /path/to/Slitherlink
chmod +x benchmarks/run_benchmarks.sh
./benchmarks/run_benchmarks.sh
```

This will:

- Test different puzzle sizes (4x4, 5x5, 6x6)
- Test thread scaling (1, 2, 4, 8 threads)
- Test CPU percentage options
- Save results to `benchmark_results.txt`

### Comprehensive Benchmark (C++ Tool)

```bash
# Build the benchmark tool
cd cmake-build-debug/benchmarks
cmake --build . --target benchmark_performance

# Run benchmarks
./benchmark_performance

# Or specify custom solver path
./benchmark_performance /path/to/slitherlink
```

Output includes:

- Individual run times
- Average, standard deviation, min, max
- CSV export for data analysis

## Metrics Tracked

- **Execution Time**: Total solver runtime
- **Thread Scaling**: Performance vs thread count
- **CPU Usage**: Efficiency at different CPU percentages
- **Puzzle Complexity**: Time scaling with puzzle size

## Analyzing Results

### Excel/LibreOffice

Import `benchmark_results.csv` and create charts for:

- Thread count vs. execution time
- Puzzle size vs. execution time
- Speedup factor (T1/Tn)

### Python Analysis

```python
import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('benchmark_results.csv')

# Plot thread scaling
df[df['Puzzle'].str.contains('5x5')].plot(x='Threads', y='Average', marker='o')
plt.xlabel('Number of Threads')
plt.ylabel('Time (seconds)')
plt.title('Thread Scaling on 5x5 Puzzle')
plt.show()
```

## Performance Baseline

Typical results on modern hardware (10-core CPU):

| Puzzle Size | Threads | Time (s) |
| ----------- | ------- | -------- |
| 4x4 Easy    | 8       | ~0.001   |
| 5x5 Medium  | 8       | ~0.05    |
| 6x6 Medium  | 8       | ~2.0     |

## Customization

Edit scripts to:

- Add more puzzle files
- Test different thread counts
- Change number of iterations
- Add custom metrics

## Continuous Integration

Integrate benchmarks into CI/CD:

```yaml
- name: Run Benchmarks
  run: |
    chmod +x benchmarks/run_benchmarks.sh
    ./benchmarks/run_benchmarks.sh

- name: Upload Results
  uses: actions/upload-artifact@v3
  with:
    name: benchmark-results
    path: benchmark_results.*
```
