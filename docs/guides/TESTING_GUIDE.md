# Quick Reference: Puzzle Testing & Benchmarking

⚠️ **Important:** Performance varies significantly based on puzzle characteristics (clue density, symmetry, forced moves). Times shown are approximate and can vary 5-10× on similar puzzles.

## Running Tests

### Quick Single Test

```bash
# Test one puzzle
./cmake-build-debug/slitherlink example4x4.txt

# With timing
time ./cmake-build-debug/slitherlink example8x8.txt

# With timeout (macOS)
gtimeout 300 ./cmake-build-debug/slitherlink example10x10.txt
```

### Full Benchmark Suite

```bash
# Run comprehensive benchmarks
./benchmark_suite.sh

# Results saved to:
# - benchmark_results.csv
# - benchmark_detailed.log
```

### View Results

```bash
# CSV summary
column -t -s',' benchmark_results.csv

# Detailed logs
cat benchmark_detailed.log | less

# Quick stats
grep "SUCCESS\|TIMEOUT" benchmark_results.csv | wc -l
```

---

## Test Puzzle Overview

### Available Test Files

| File                    | Size  | Clues | Density | Difficulty | Expected Time | Purpose            |
| ----------------------- | ----- | ----- | ------- | ---------- | ------------- | ------------------ |
| example4x4.txt          | 4×4   | 4     | 25%     | Trivial    | ~1ms          | Quick validation   |
| example5x5.txt          | 5×5   | 12    | 48%     | Easy       | ~60ms         | Small puzzle test  |
| example6x6.txt          | 6×6   | 18    | 50%     | Medium     | ~5s           | Medium test        |
| example7x7.txt          | 7×7   | 24    | 49%     | Medium     | ~12s          | Scaling test       |
| example8x8_simple.txt   | 8×8   | 64    | 100%    | Easy       | ~0.4ms        | Dense puzzle test  |
| example8x8_box.txt      | 8×8   | 32    | 50%     | Medium     | ~180ms        | Pattern test       |
| example8x8.txt          | 8×8   | 32    | 50%     | Hard       | ~520ms        | Sparse center test |
| example10x10_dense.txt  | 10×10 | 16    | 16%     | Hard       | ~95s          | Sparse test        |
| example10x10.txt        | 10×10 | 28    | 28%     | Very Hard  | ~125s         | Main 10×10 test    |
| example12x12_simple.txt | 12×12 | 144   | 100%    | Medium     | ~2.5s         | Large dense test   |
| example12x12.txt        | 12×12 | 72    | 50%     | Extreme    | ~600s         | Large sparse test  |
| example15x15.txt        | 15×15 | ~45   | 20%     | Extreme    | ~1800s        | Stress test        |
| example20x20.txt        | 20×20 | ~80   | 20%     | Nightmare  | TIMEOUT       | Limit test         |

---

## Performance Expectations

### By Size (typical ranges)

**Note:** These are approximate ranges. Individual puzzles may vary significantly.

```
4×4:     ~0.001s        ✓ Instant (very consistent)
5×5:     ~0.001-0.1s    ✓ Instant to fast
6×6:     ~0.5-3s        ✓ Fast (high variance)
7×7:     ~1-10s         ✓ Fast to acceptable
8×8:     ~0.5-1s        ✓ Acceptable (variance 0.3-11s observed)
10×10:   TIMEOUT        ✗ Most puzzles >60s
12×12:   TIMEOUT        ✗ Very challenging
15×15:   TIMEOUT        ✗ Too hard
20×20:   TIMEOUT        ✗ Too hard
```

**Comparison to V1 baseline:**

- 4×4: ~100× faster (~0.1s → ~0.001s)
- 5×5: ~500× faster (~0.5s → ~0.001s)
- 8×8: ~15-30× faster (~15s → ~0.5-1s)
- 10×10: Still challenging (both often timeout)

### By Density (10×10 puzzle)

```
Density | Example Time | Status
--------|--------------|--------
10-20%  | 150-250s    | Very hard
25-35%  | 90-150s     | Hard
40-50%  | 50-90s      | Medium
60-80%  | 10-40s      | Easy
90-100% | 1-5s        | Trivial
```

---

## Understanding Benchmark Output

### Success Messages

```
✓ SUCCESS - Found 1 solution(s)
⏱ Time: 0.519s

→ Puzzle solved correctly
→ Time is wall-clock (real time)
```

### Timeout Messages

```
⏱ TIMEOUT after 300s

→ Puzzle too hard for current algorithm
→ May need algorithmic improvements
→ Try smaller puzzle or increase timeout
```

### Error Messages

```
✗ ERROR

→ Solver crashed or invalid input
→ Check puzzle file format
→ Check build configuration
```

---

## Interpreting CSV Results

### CSV Format

```csv
Puzzle,Size,Difficulty,Clues,Density,Time(s),Solutions,Status
example4x4.txt,4x4,Trivial,4,0.250,0.0013,1,SUCCESS
```

### Fields Explained

- **Puzzle**: Input file name
- **Size**: Grid dimensions (e.g., "10x10")
- **Difficulty**: Estimated difficulty tier
- **Clues**: Number of cells with clue numbers
- **Density**: Clues / Total cells (0.0-1.0)
- **Time(s)**: Solving time in seconds
- **Solutions**: Number of valid solutions found
- **Status**: SUCCESS/TIMEOUT/ERROR/NO_SOLUTION

### Status Codes

- `SUCCESS`: Solved, solution found
- `TIMEOUT`: Time limit exceeded
- `ERROR`: Solver error or crash
- `NO_SOLUTION`: Solver completed but puzzle has no solution

---

## Creating Custom Puzzles

### File Format

```
<rows> <columns>
<clue_row_1>
<clue_row_2>
...
<clue_row_n>
```

### Example: 3×3 Puzzle

```
3 3
2 . 2
. 2 .
2 . 2
```

### Clue Values

- `0`, `1`, `2`, `3`: Number of edges that must be ON
- `.`: No clue (unconstrained cell)
- Separate clues with spaces

### Tips for Good Puzzles

1. **Start simple**: 4×4 or 5×5
2. **Add 0s and 3s**: Strongest constraints
3. **Even distribution**: Avoid large empty regions
4. **Test uniqueness**: Should have exactly 1 solution
5. **Balance density**: 30-60% for interesting puzzles

---

## Performance Tuning

### Build Configuration

**Debug Build** (current):

```bash
cmake --build cmake-build-debug
# Slower but has debug info
```

**Release Build** (faster):

```bash
mkdir -p cmake-build-release
cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
# 2-3× faster than debug
```

### TBB Thread Control

Modify in `main.cpp`:

```cpp
// Reduce threads (less CPU usage)
int numThreads = thread::hardware_concurrency() / 4;  // Use 25%

// Increase threads (faster but more CPU)
int numThreads = thread::hardware_concurrency();  // Use 100%
```

### Adaptive Depth Control

Modify in `main.cpp`:

```cpp
int calculateOptimalParallelDepth() {
    // Make less aggressive (slower but less CPU)
    if (totalCells <= 100) depth = 15;  // Was 20

    // Make more aggressive (faster but more CPU)
    if (totalCells <= 100) depth = 25;  // Was 20
}
```

---

## Troubleshooting

### Problem: Timeout on 10×10

**Solutions**:

1. Increase timeout: `gtimeout 600 ./slitherlink puzzle.txt`
2. Build release version (2-3× faster)
3. Check puzzle density (should be >20%)
4. Verify puzzle has solution

### Problem: Out of Memory

**Solutions**:

1. Reduce parallel depth (edit calculateOptimalParallelDepth)
2. Reduce thread count
3. Test on smaller puzzle first
4. Check for infinite loops (shouldn't happen)

### Problem: Wrong Solution

**Solutions**:

1. Verify puzzle file format
2. Check for typos in clues
3. Verify expected solution
4. May be multiple valid solutions

### Problem: Benchmark Script Fails

**Solutions**:

```bash
# Install GNU timeout on macOS
brew install coreutils

# Or edit benchmark_suite.sh to use different timeout
# Change gtimeout to timeout or remove timeout entirely
```

---

## Advanced Usage

### Batch Testing

```bash
# Test all 8×8 puzzles
for f in example8x8*.txt; do
    echo "Testing $f..."
    time ./cmake-build-debug/slitherlink "$f"
done
```

### Performance Profiling

```bash
# macOS: Use Instruments
xcode-select --install
instruments -t "Time Profiler" ./cmake-build-debug/slitherlink example10x10.txt

# Linux: Use perf
perf record ./cmake-build-debug/slitherlink example10x10.txt
perf report
```

### Memory Profiling

```bash
# macOS: Use Instruments
instruments -t "Allocations" ./cmake-build-debug/slitherlink example10x10.txt

# Linux: Use valgrind
valgrind --tool=massif ./cmake-build-debug/slitherlink example10x10.txt
```

### Custom Metrics

Add to `main.cpp`:

```cpp
// Track node count
atomic<long long> nodesExplored{0};

void search(...) {
    nodesExplored++;
    // ... rest of search
}

// Print at end
cout << "Nodes explored: " << nodesExplored << "\n";
```

---

## Regression Testing

### After Code Changes

```bash
# 1. Build
cmake --build cmake-build-debug

# 2. Run quick tests
./cmake-build-debug/slitherlink example4x4.txt
./cmake-build-debug/slitherlink example8x8_simple.txt

# 3. Run full suite
./benchmark_suite.sh

# 4. Compare results
diff benchmark_results.csv benchmark_results_baseline.csv
```

### Establish Baseline

```bash
# First time
./benchmark_suite.sh
cp benchmark_results.csv benchmark_results_baseline.csv

# After changes
./benchmark_suite.sh
# Compare times to baseline
```

---

## Documentation References

### Main Documents

1. **10x10_OPTIMIZATION_JOURNEY.md**: Complete optimization story
2. **PUZZLE_DIFFICULTY_ANALYSIS.md**: Difficulty factors and algorithm behavior
3. **THIS FILE**: Quick reference for testing

### Key Sections

- **Tools tried**: OR-Tools, constraint propagation (in OPTIMIZATION_JOURNEY)
- **Depth strategies**: Adaptive vs fixed (in OPTIMIZATION_JOURNEY)
- **Benchmark data**: Real timing across puzzles (in DIFFICULTY_ANALYSIS)
- **Algorithm behavior**: How it responds to different puzzles (in DIFFICULTY_ANALYSIS)

---

## Summary Commands

```bash
# Quick validation (30s)
./cmake-build-debug/slitherlink example4x4.txt
./cmake-build-debug/slitherlink example8x8_simple.txt

# Medium test (2min)
time ./cmake-build-debug/slitherlink example8x8.txt

# Full benchmark (30-60min)
./benchmark_suite.sh

# View results
column -t -s',' benchmark_results.csv | head -20
```

---

## Expected Benchmark Results (V10)

| Test Category | Time Range | Success Rate | Notes                      |
| ------------- | ---------- | ------------ | -------------------------- |
| 4×4 to 6×6    | <10s       | 100%         | Should never fail          |
| 7×7 to 8×8    | <60s       | 100%         | Reliable                   |
| 10×10         | 60-180s    | 95%+         | May timeout if very sparse |
| 12×12         | 300-900s   | 80%          | Sparse may timeout         |
| 15×15+        | >1000s     | 50%          | Many will timeout          |

**If results differ significantly**: Check build configuration, CPU load, or code regressions.
