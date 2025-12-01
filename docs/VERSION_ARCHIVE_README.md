# Slitherlink Solver - Version Archive

**Total Code:** 9,006 lines across 8 historical versions  
**Current:** 1,023 lines (main.cpp, derived from V10)

## Overview

This archive documents the real code evolution preserved in `tests/old_versions/`. The development followed a clear architectural path:

- **V1–V9:** `std::async` era (incremental improvements to parallel search)
- **V10:** Intel TBB rewrite (architectural shift to task-based parallelism)
- **main.cpp:** Current production code (refined V10)

> ⚠️ **Note:** Some headers in V7/V9 mention TBB conceptually, but these versions still use `std::async` throughout. TBB appears only in V10.

## Folder Layout

```
tests/old_versions/
├── version.txt               ← Short notes for V1–V9 (async era)
├── v01_baseline.cpp          ← V1: baseline std::async
├── v02_threadpool.cpp        ← V2: manual thread control over async
├── v03_from_history.cpp      ← V3: pruning/ordering tweaks
├── v04_from_history.cpp      ← V4: smarter pruning + forced moves
├── v05_from_history.cpp      ← V5: hybrid pruning + parallel search
├── v07_from_history.cpp      ← V7: depth/heuristic tuning (still async)
├── v09_from_history.cpp      ← V9: final async polish
├── v10_final.cpp             ← V10: first TBB implementation
└── main.cpp                  ← Current production (derived from V10)
```

> Note: Some headers in V7/V9 mention TBB conceptually, but the code in V1–V9 uses `std::async` throughout. TBB appears only in V10.

## Quick Version Snapshot

> V6 and V8 were incremental async refinements; they do not have separate source files in the archive and are covered by the surrounding versions/notes.

- **V1 (Baseline)**: Unbounded `std::async`, minimal heuristics. Good for comparison.
- **V2 (Thread Control)**: Adds task limiting/queueing to curb async explosion.
- **V3–V5 (Pruning Focus)**: Better edge ordering, early constraint checks, and forced-move propagation; still async-based.
- **V7 (Depth/Heuristic Tuning)**: More tuning of depth limits and heuristics; still async.
- **V9 (Async Finish)**: Last std::async iteration before the rewrite.
- **V10 (TBB Rewrite)**: Introduces `tbb::task_group`, `tbb::task_arena`, adaptive depth, and cleaner heuristics. Basis for `main.cpp`.

## How to Build Specific Versions

```bash
# Example: build V1 baseline (no TBB needed)
g++ -std=c++17 -O3 tests/old_versions/v01_baseline.cpp -o slitherlink_v01

# Example: build V10 with TBB (macOS/Homebrew paths)
g++ -std=c++17 -O3 -DUSE_TBB \
  -I/opt/homebrew/include -L/opt/homebrew/lib -ltbb \
  tests/old_versions/v10_final.cpp -o slitherlink_v10
```

## Benchmarking Tips

⚠️ **Performance varies significantly** based on puzzle characteristics (clue density, symmetry, forced moves).

**Expected Results (V10/main.cpp):**

- 4×4: ~0.001s (~100× improvement)
- 5×5: ~0.001s (~500× improvement)
- 8×8: ~0.5-1s with variance (0.3s-11s observed) (~15-30× improvement)
- 10×10: Most timeout (>60s)

**Best Practices:**

- Run each executable against the same puzzle set to avoid mixing async/TBB results
- Record thread counts and puzzle characteristics (size, clue density) with timings
- Test multiple puzzles of each size to capture performance variance
- Use scripts/test_solver.sh for consistent automated testing
- If documentation claims gradual TBB adoption before V10, treat it as historical commentary

## Learning Path

1. Read `v01_baseline.cpp` for the simplest solver structure.
2. Compare `v02_threadpool.cpp` to see thread limiting over async.
3. Skim `v04_from_history.cpp` and `v05_from_history.cpp` to understand pruning and forced-move logic.
4. Jump to `v10_final.cpp` and `main.cpp` to study the TBB rewrite.

This README is now consistent with the repository contents and should match what you see in the code.
