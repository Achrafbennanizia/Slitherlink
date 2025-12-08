# Slitherlink Solver

Parallel Slitherlink solver using backtracking, heuristics, and Intel oneAPI TBB. This README reflects the current repository layout (CLI in `apps/slitherlink_cli`, puzzles in `puzzles/samples`).

## Overview
- Language: C++17
- Parallelism: Intel oneAPI TBB (optional but recommended)
- Entry point: `apps/slitherlink_cli/main.cpp`
- Solver core: `src/solver/` with supporting `src/core`, `src/io`, `src/factory`, `src/utils`
- Historical versions: `tests/old_versions/` (V1–V10 snapshots)

## Project Structure (current)
```
Slitherlink/
├── apps/slitherlink_cli/    # CLI sources (main.cpp, backups)
├── src/                     # Solver modules
│   ├── core/                # Grid/state primitives
│   ├── solver/              # Heuristics, propagation, validation
│   ├── io/                  # Reader/collector/printer
│   ├── factory/             # Factories and facade
│   └── utils/               # Config parsing
├── include/                 # Public headers (SOLID experiments)
├── puzzles/samples/         # Sample puzzles (5x5, 7x7, 12x12 variants)
├── scripts/                 # Benchmarks and helpers
├── tests/                   # Unit tests + old version archive
├── docs/                    # Guides, analysis, history, improvement options
├── CMakeLists.txt           # Build configuration
└── Doxyfile                 # Doxygen config (optional)
```

## Build & Run
```bash
# Configure (Debug)
cmake -S . -B cmake-build-debug -DUSE_TBB=ON

# Build
cmake --build cmake-build-debug

# Run a sample puzzle
./cmake-build-debug/slitherlink puzzles/samples/example5x5.txt
./cmake-build-debug/slitherlink puzzles/samples/example7x7.txt
```

Release build (faster for larger puzzles):
```bash
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -DUSE_TBB=ON
cmake --build cmake-build-release
```

## Puzzles Included (current repo)
- 5×5: `example5x5_{easy,medium,hard,extreme}.txt`
- 7×7: `example7x7_{easy,medium,hard,extreme}.txt`
- 12×12: `example12x12_{easy,medium,hard,extreme,simple}.txt`

Older docs referencing 4×4/8×8/10×10 files do not match the current tree.

## Testing & Benchmarking
- Quick manual checks: run the sample puzzles above.
- Scripts (see `scripts/`):
  - `quick_sample_benchmark.sh` — small representative run
  - `benchmark_suite.sh` — broader benchmark (longer)
  - `test_originals.sh`, `validate_and_benchmark.sh` — targeted suites
- If built with `-DSLITHERLINK_BUILD_TESTS=ON`, run `ctest --output-on-failure`.
See `docs/guides/TESTING_GUIDE.md` for details.

## Documentation Map
- Docs hub: `docs/README.md`
- Navigation & testing: `docs/guides/`
- Architecture: `docs/developer/ARCHITECTURE.md`
- Performance/algorithms: `docs/analysis/`
- History/migration: `docs/history/`
- Improvement backlog: `docs/IMPROVEMENT_OPTIONS.md`

## Notes on Versions and History
- V1–V9 (std::async era) and V10 (first TBB rewrite) are preserved in `tests/old_versions/`.
- SOLID-oriented headers under `include/` and `src/factory` are experimental; the CLI currently uses the monolithic path.
- Some historical documents describe older layouts; use this README and `docs/README.md` as the source of truth for the current structure.
