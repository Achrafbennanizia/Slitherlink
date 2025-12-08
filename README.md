# Slitherlink Solver

High-performance parallel Slitherlink puzzle solver using backtracking, constraint propagation, and Intel oneAPI TBB for parallel search.

## Overview

- **Language**: C++17
- **Parallelism**: Intel oneAPI TBB (optional but recommended for best performance)
- **Entry Point**: `apps/slitherlink_cli/main.cpp` (monolithic solver)
- **Solver Core**: Modular architecture in `src/` (core, solver, io, factory, utils)
- **Headers**: Organized by module in `include/` (core, solver, interfaces, io, factory, utils)
- **Historical Versions**: Preserved in `BackUp_oldCode/old_versions/` (V1–V10 snapshots)

## Project Structure

```
Slitherlink/
├── apps/
│   └── slitherlink_cli/     # CLI application
│       └── main.cpp         # Main solver (monolithic, self-contained)
├── include/                 # Public headers (organized by module)
│   ├── core/                # Grid, State, Edge, Solution
│   ├── solver/              # Solver, heuristics, propagators, validators
│   ├── interfaces/          # Abstract interfaces (IHeuristic, IPropagator, IValidator)
│   ├── io/                  # GridReader, SolutionCollector, SolutionPrinter
│   ├── factory/             # SolverFactory, SlitherlinkSolver facade
│   └── utils/               # Config utilities
├── src/                     # Implementation files (mirrors include/ structure)
│   ├── core/                # Grid.cpp, State.cpp
│   ├── solver/              # Solver.cpp, heuristics, propagators, validators
│   ├── io/                  # I/O implementations
│   ├── factory/             # Factory implementations
│   └── utils/               # Utility implementations
├── puzzles/samples/         # Sample puzzles organized by size
│   ├── 4x4/, 6x6/, 8x8/, 10x10/, 15x15/, 20x20/
│   └── *.txt                # Additional samples (5x5, 7x7, 12x12)
├── tests/
│   ├── unit/                # Unit tests
│   ├── integration/         # Integration tests (placeholder)
│   └── benchmarks/          # Performance benchmarks
├── docs/                    # Documentation (organized by audience)
│   ├── README.md            # Documentation hub
│   ├── user/                # User guides (USER_GUIDE.md, QUICK_START.md)
│   ├── developer/           # Developer documentation (ARCHITECTURE.md, SOLID docs)
│   ├── technical/           # Technical details & optimizations
│   ├── guides/              # Navigation & testing guides
│   ├── analysis/            # Performance analysis & optimization journey
│   └── history/             # Project history & migration logs
├── BackUp_oldCode/          # Archived historical versions (V1-V10)
│   └── old_versions/        # Complete version history with evolution notes
├── scripts/                 # Build and benchmark scripts
├── benchmarks/              # Benchmark utilities
├── results/                 # Benchmark results
├── CMakeLists.txt           # Build configuration
└── Doxyfile                 # API documentation generator config
```

## Build & Run

```bash
# Configure & Build (Release recommended for best performance)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DUSE_TBB=ON
cmake --build build -j8

# Run sample puzzles
./build/slitherlink puzzles/samples/4x4/example4x4.txt
./build/slitherlink puzzles/samples/10x10/example10x10.txt
```

Debug build (for development):

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug -DUSE_TBB=ON
cmake --build build-debug
```

## Puzzles Included

Puzzles are organized in `puzzles/samples/` by grid size:

- **4×4**: 5 puzzles in `puzzles/samples/4x4/`
- **6×6**: 5 puzzles in `puzzles/samples/6x6/`
- **8×8**: 7 puzzles in `puzzles/samples/8x8/`
- **10×10**: 6 puzzles in `puzzles/samples/10x10/`
- **15×15**: 5 puzzles in `puzzles/samples/15x15/`
- **20×20**: 6 puzzles in `puzzles/samples/20x20/`
- **Additional**: 5×5, 7×7, 12×12 variants in `puzzles/samples/*.txt`

Example runs:

```bash
./build/slitherlink puzzles/samples/4x4/example4x4.txt
./build/slitherlink puzzles/samples/10x10/example10x10.txt
./build/slitherlink puzzles/samples/example7x7.txt
```

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
