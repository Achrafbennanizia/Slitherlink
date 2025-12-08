# Quick Start (Current Layout)

Use this as the fastest path to build and run the solver with the present repository structure.

## Build
```bash
# Configure (Debug)
cmake -S . -B cmake-build-debug -DUSE_TBB=ON

# Build
cmake --build cmake-build-debug
```

Release build (faster for large puzzles):
```bash
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -DUSE_TBB=ON
cmake --build cmake-build-release
```

## Run
```bash
# Debug build
./cmake-build-debug/slitherlink puzzles/samples/example5x5.txt

# Release build
./cmake-build-release/slitherlink puzzles/samples/example7x7.txt
```

## Sample puzzles available
- 5×5: `example5x5_{easy,medium,hard,extreme}.txt`
- 7×7: `example7x7_{easy,medium,hard,extreme}.txt`
- 12×12: `example12x12_{easy,medium,hard,extreme,simple}.txt`

Older docs mentioning 4×4/8×8/10×10 files do not match the current tree.

## Handy options
```bash
./cmake-build-debug/slitherlink --help
./cmake-build-debug/slitherlink puzzles/samples/example7x7.txt --threads 4
./cmake-build-debug/slitherlink puzzles/samples/example7x7.txt --all
./cmake-build-debug/slitherlink puzzles/samples/example7x7.txt --timeout 60
```

## Where to go next
- Full user guide: `docs/user/USER_GUIDE.md`
- Testing and benchmarks: `docs/guides/TESTING_GUIDE.md`
- Architecture overview: `docs/developer/ARCHITECTURE.md`
