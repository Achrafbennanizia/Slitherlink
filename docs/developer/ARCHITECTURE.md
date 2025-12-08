# Project Architecture (Current)

This document describes the current file organization. Use this as the source of truth for layout; some historical docs still reference older structures.

## Overview
- Language: C++17
- Build: CMake
- Parallelism: Intel oneAPI TBB (optional, recommended)
- CLI entrypoint: `apps/slitherlink_cli/main.cpp`
- Solver modules: `src/`
- Public/experimental SOLID headers: `include/`
- Samples: `puzzles/samples/`

## Directory Structure (current)
```
Slitherlink/
├── apps/slitherlink_cli/    # CLI sources (main.cpp, alternates/backups)
├── src/
│   ├── core/                # Grid/state primitives
│   ├── solver/              # Search, heuristics, propagation, validation
│   ├── io/                  # Grid reader, solution collector/printer
│   ├── factory/             # Factories and facade helpers
│   └── utils/               # Config handling
├── include/                 # Public headers (SOLID-oriented experiments)
├── puzzles/samples/         # Sample puzzles (5x5, 7x7, 12x12 variants)
├── scripts/                 # Benchmarks and helpers
├── tests/                   # Unit tests and old version archive
│   ├── unit/                # Basic unit tests
│   └── old_versions/        # V1–V10 snapshots (std::async → TBB)
├── docs/                    # Guides, analysis, history, improvements
├── CMakeLists.txt           # Build configuration
└── Doxyfile                 # Doxygen config
```

## Modules (high level)
- **core**: grid and state data structures.
- **solver**: edge selection, constraint propagation, validation, search driver.
- **io**: parsing input puzzles, collecting and printing solutions.
- **factory**: SOLID/facade helpers (experimental; CLI uses monolithic flow).
- **utils**: config parsing/validation.

## Tests
- Unit tests live in `tests/unit/` (enable with `-DSLITHERLINK_BUILD_TESTS=ON` and run `ctest`).
- Historical versions in `tests/old_versions/` for comparison/benchmarks.

## Documentation
- Hub: `docs/README.md`
- Navigation/testing: `docs/guides/`
- Architecture detail: this file and `docs/developer/SOLID_ARCHITECTURE.md` (experimental)
- Performance/algorithms: `docs/analysis/`
- History/migration: `docs/history/`
- Improvement backlog: `docs/IMPROVEMENT_OPTIONS.md`

## Notes on accuracy
- Older docs may mention root-level `main.cpp` or 4×4/8×8/10×10 puzzles; current layout uses `apps/slitherlink_cli` and `puzzles/samples` (5×5, 7×7, 12×12 variants).
- Treat this file and the root `README.md` as canonical for structure.
