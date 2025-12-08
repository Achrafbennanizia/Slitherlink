# 🗺️ Documentation Navigation Guide

This guide points to the current documentation and code layout. It matches the repository as it exists now (CLI in `apps/slitherlink_cli`, puzzles in `puzzles/samples`).

## Where to start
- Quick start: `docs/user/QUICK_START.md`
- How to use the solver: `docs/user/USER_GUIDE.md`
- Architecture overview: `docs/developer/ARCHITECTURE.md`
- Testing and benchmarking: `docs/guides/TESTING_GUIDE.md`
- Performance deep dives: `docs/analysis/` (TBB integration, 10x10 journey, difficulty analysis)
- Migration/history: `docs/history/ARCHITECTURE_MIGRATION_SUMMARY.md`, `docs/history/REFACTORING_CHANGELOG.md`
- Improvement backlog: `docs/IMPROVEMENT_OPTIONS.md`

## Directory map (docs)
```
docs/
├── README.md                → Documentation hub
├── guides/                  → Navigation + testing guides
├── user/                    → Quick start and user guide
├── developer/               → Architecture + SOLID experiments
├── analysis/                → Performance and algorithm write-ups
├── technical/               → Optimization reports and comparisons
└── history/                 → Migration and evolution logs (archival)
```

## Reading paths
- **Fast start (30–45 min)**: `docs/user/QUICK_START.md` → `docs/developer/ARCHITECTURE.md` → skim `apps/slitherlink_cli/main.cpp`
- **Performance focus (60–90 min)**: `docs/analysis/TBB_INTEGRATION_STORY.md` → `docs/analysis/10x10_OPTIMIZATION_JOURNEY.md`
- **History (45–60 min)**: `docs/history/ARCHITECTURE_MIGRATION_SUMMARY.md` → `docs/history/REFACTORING_CHANGELOG.md`

## Quick access by topic
- Parallelization (TBB): `docs/analysis/TBB_INTEGRATION_STORY.md`; code in `src/solver/Solver.cpp`
- Heuristics & propagation: `docs/analysis/PUZZLE_DIFFICULTY_ANALYSIS.md`; code in `src/solver/SmartHeuristic.cpp` and `src/solver/OptimizedPropagator.cpp`
- Architecture layout: `docs/developer/ARCHITECTURE.md`; CLI entrypoint `apps/slitherlink_cli/main.cpp`
- Performance stories: `docs/analysis/10x10_OPTIMIZATION_JOURNEY.md`
- Migration context: `docs/history/ARCHITECTURE_MIGRATION_SUMMARY.md`

## Search tips
```bash
# Search docs for a term
rg "parallel" docs

# Find puzzle references
rg "puzzles/samples" docs

# List headings in a large doc
rg "^##" docs/analysis/TBB_INTEGRATION_STORY.md
```

## Handy commands
```bash
# Build (Debug by default)
cmake --build cmake-build-debug

# Run a sample puzzle
./cmake-build-debug/slitherlink puzzles/samples/example5x5.txt

# Run tests (when built with -DSLITHERLINK_BUILD_TESTS=ON)
ctest --output-on-failure
```
