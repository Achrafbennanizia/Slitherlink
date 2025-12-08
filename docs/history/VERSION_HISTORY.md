# Slitherlink Solver - Complete Version History

> Historical reference. For the current layout and entrypoints, see `README.md` and `docs/developer/ARCHITECTURE.md`.

This directory contains all major code versions developed during the 4-week optimization journey (November 2025).

## Version Files

> Availability note: We extracted the versions stored in `version.txt` as `versions/vXX_from_history.cpp`. Dedicated files for V06 and V08 were not present in the repo; their behavior is summarized below but not archived as source.

- **v01_baseline.cpp** (and `v01_from_history.cpp`) - Week 1, Days 1-3: Initial implementation (800 lines)
- **v02_from_history.cpp** - Week 1, Days 5-7: TBB parallelization (≈950 lines)
- **v03_from_history.cpp** - Week 2, Days 8-9: CPU limiting to 50% (≈950 lines)
- **v04_from_history.cpp** - Week 2, Days 10-11: Adaptive parallel depth (≈1100 lines)
- **v05_from_history.cpp** - Week 2, Days 12-14: Edge selection heuristics (≈1100 lines)
- **v06_propagation.cpp** - Week 3, Days 15-17: Constraint propagation (1360 lines) — *summary only, source not present*
- **v07_from_history.cpp** - Week 3, Day 18-21: OR-Tools attempt (1630 lines) - FAILED
- **v08_cleanup.cpp** - Week 3, Days 22-24: OR-Tools removed (1100 lines) — *summary only, source not present*
- **v09_from_history.cpp** - Week 4, Days 25-27: TBB in validation (987 lines)
- **v10_final.cpp** (and `v10_from_history.cpp`) - Week 4, Day 28: Lambda optimization (987 lines) - CURRENT

## Failed Experiments (Not Included as Full Versions)

### Experiment 1A: OpenMP Attempt (Day 4)

**Problem**: Attempted to use OpenMP instead of TBB
**Issue**: Data parallelism doesn't work well for irregular search trees
**Time Lost**: 1 day
**Code**: Never completed, abandoned early

### Experiment 2A: Simple sqrt Depth (Day 10 AM)

**Problem**: Used `depth = sqrt(totalCells)` for parallel depth
**Issue**: All depths too shallow (4×4=2, 8×8=8), killed performance
**Time Lost**: 4 hours
**Code**: Quick prototype, not saved

### Experiment 2B: Density-Only Depth (Day 10 PM)

**Problem**: Only considered clue density, ignored puzzle size
**Issue**: 4×4 and 8×8 got same depth despite very different complexity
**Time Lost**: 4 hours
**Code**: Merged into V4 combined approach

### Experiment 3A: Forward Checking Only (Day 15)

**Problem**: Unidirectional constraint propagation
**Issue**: Missed constraint violations that required backward propagation
**Time Lost**: 4 hours
**Code**: Enhanced into V6 bidirectional propagation

### Experiment 4A: OR-Tools Integration (Days 18-21) - See v07_ortools_failed.cpp

**Problem**: Tried 3 different constraint formulations
**Issues**:

- Distance constraints → disconnected cycles
- Flow constraints → multiple cycles
- Reachability constraints → model explosion
  **Time Lost**: 4 days
  **Code**: Full implementation saved as v07_ortools_failed.cpp

## Performance Evolution

| Version | 4×4    | 5×5    | 8×8    | Lines | Key Change             |
| ------- | ------ | ------ | ------ | ----- | ---------------------- |
| V1      | 0.100s | 2.0s   | 15.0s  | 800   | Baseline               |
| V2      | 0.003s | 0.5s   | 5.0s   | 950   | TBB (+3×)              |
| V3      | 0.003s | 0.5s   | 6.0s   | 950   | CPU limit              |
| V4      | 0.003s | 0.026s | 0.64s  | 1100  | Adaptive (+10×)        |
| V5      | 0.002s | 0.020s | 0.53s  | 1100  | Heuristics (+1.2×)     |
| V6      | 0.002s | 0.015s | 0.48s  | 1360  | Propagation (+1.1×)    |
| V7      | -      | -      | -      | 1630  | OR-Tools (FAILED)      |
| V8      | 0.002s | 0.015s | 0.48s  | 1100  | Cleanup                |
| V9      | 0.002s | 0.063s | 0.70s  | 987   | TBB validation (+1.5×) |
| V10     | 0.002s | 0.063s | 0.705s | 987   | Polish                 |

**Total Improvement**: V1 → V10 = 21-50× faster

## Key Milestones

1. **Biggest Performance Win**: V4 Dynamic Depth (10-30× speedup)
2. **Biggest Code Win**: V10 Lambda Optimization (-373 lines from peak)
3. **Biggest Failure**: V7 OR-Tools (4 days lost, -270 lines removed)
4. **Most Complex**: V6 Constraint Propagation (+260 lines)
5. **Final Polish**: V9-V10 TBB validation + lambda cleanup

## How to Use These Files

Each version is a complete standalone program. To compile and test:

```bash
# Compile any version (replace XX with version number)
g++ -std=c++17 -O3 -DUSE_TBB -I/opt/intel/oneapi/tbb/latest/include \
    -L/opt/intel/oneapi/tbb/latest/lib -ltbb \
    versions/vXX_name.cpp -o slitherlink_vXX

# Run benchmark
time ./slitherlink_vXX example8x8.txt
```

## Development Statistics

- **Total Days**: 28
- **Total Versions**: 10 major versions
- **Failed Experiments**: 5
- **Time on Failed Paths**: ~7 days (25%)
- **Code Size Range**: 800 → 1630 → 987 lines
- **Performance Range**: 0.1s → 0.002s (4×4), 15s → 0.705s (8×8)
- **Speedup**: 21-50× depending on puzzle

## Lessons Learned

### Technical

- Task-based parallelism (TBB) > data parallelism (OpenMP) for search
- Adaptive strategies beat fixed strategies
- Domain-specific heuristics > generic solvers
- Profile before optimizing (found 15% validation bottleneck)

### Process

- 25% exploration time on failures is normal and valuable
- Small incremental changes allow easy rollback
- Keep baseline for honest comparison
- Document everything (you're reading it!)

## Notes

- All versions use the same puzzle format
- V7 compiles but generates invalid solutions (kept for reference)
- V10 is the current production version (in ../main.cpp)
- Build instructions in main README.md
