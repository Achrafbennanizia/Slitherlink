# Testing & Benchmarking Guide

Practical steps to build, run, and validate the solver with the current repository layout.

## 1) Build
```bash
# Configure (if not already)
cmake -S . -B cmake-build-debug -DUSE_TBB=ON

# Build
cmake --build cmake-build-debug
```

For faster runs, build Release:
```bash
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -DUSE_TBB=ON
cmake --build cmake-build-release
```

## 2) Quick manual tests
```bash
# Small puzzle sanity checks
./cmake-build-debug/slitherlink puzzles/samples/example5x5.txt
./cmake-build-debug/slitherlink puzzles/samples/example7x7.txt

# Harder variants
./cmake-build-debug/slitherlink puzzles/samples/example5x5_extreme.txt
./cmake-build-debug/slitherlink puzzles/samples/example7x7_extreme.txt

# Large / stress
./cmake-build-debug/slitherlink puzzles/samples/example12x12.txt
./cmake-build-debug/slitherlink puzzles/samples/example12x12_extreme.txt
```

Expected behavior:
- 5×5 and 7×7 variants should finish quickly.
- 12×12 variants may take significantly longer; prefer Release builds.

## 3) Benchmark scripts
Scripts live in `scripts/`. Common options:
```bash
# Small representative run
./scripts/quick_sample_benchmark.sh

# Full suite (longer)
./scripts/benchmark_suite.sh

# Validate originals (if present)
./scripts/test_originals.sh
```
Outputs typically include CSV/log files in the working directory; review them to confirm SUCCESS/TIMEOUT counts.

## 4) Running tests
If configured with `-DSLITHERLINK_BUILD_TESTS=ON`, run:
```bash
ctest --output-on-failure
```

## 5) Puzzle inventory (current repo)
- `puzzles/samples/example5x5*.txt` (easy → extreme)
- `puzzles/samples/example7x7*.txt` (easy → extreme)
- `puzzles/samples/example12x12*.txt` (easy → extreme)

Keep commands aligned with these names; older docs referencing `example4x4`/`example8x8`/`example10x10` do not match the current tree.

## 6) Tips
- Use Release builds for larger puzzles.
- If TBB is unavailable, ensure CMake reports it clearly; runtimes will differ without it.
- Capture baseline CSV from a benchmark run to compare future changes.
