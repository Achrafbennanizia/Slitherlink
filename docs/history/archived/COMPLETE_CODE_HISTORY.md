# Complete Code History (Aligned with Repository)

> Archived document. Current layout and entrypoints: `README.md`, `docs/developer/ARCHITECTURE.md`.

**Generated:** December 2025  
**Project:** High-Performance Slitherlink Solver  
**Total Code:** 1,023 lines (main.cpp) + 9,006 lines (historical versions)  
**Repository:** [Achrafbennanizia/Slitherlink](https://github.com/Achrafbennanizia/Slitherlink)

## Purpose

This document provides an accurate, conflict-free summary of the actual code evolution preserved in this repository. It corrects earlier documentation that suggested gradual TBB adoption across versions—the reality is simpler: V1-V9 used `std::async`, V10 introduced TBB.

## Source of Truth

- **Historical code:** `tests/old_versions/` (9,006 lines across 8 files)
- **Version notes:** `tests/old_versions/version.txt`
- **Current production:** `main.cpp` (1,023 lines, derived from `v10_final.cpp`)

## High-Level Timeline

| Version | Date (relative) | Concurrency Model   | Focus                                    | File(s)                                        |
| ------- | --------------- | ------------------- | ---------------------------------------- | ---------------------------------------------- |
| V1      | Day 1           | std::async (no cap) | Baseline solver                          | `v01_baseline.cpp`                             |
| V2      | Day 2           | std::async + limits | Manual thread control / queue experiment | `v02_threadpool.cpp`                           |
| V3      | Days 3–4        | std::async          | Pruning and better branch ordering       | `v03_from_history.cpp`, `v04_from_history.cpp` |
| V5      | Days 5–6        | std::async          | Hybrid pruning + parallel search         | `v05_from_history.cpp`                         |
| V6–V8   | Days 7–9        | std::async          | Iterative refinement (depth, heuristics) | Reflected in `v07_from_history.cpp`            |
| V9      | Day 10          | std::async          | Final async cleanup                      | `v09_from_history.cpp`                         |
| V10     | Day 11+         | Intel TBB           | Task-based rewrite (production)          | `v10_final.cpp`, `main.cpp`                    |

> Some headers inside V7/V9 mention TBB conceptually. The source code still uses `std::async`; TBB first appears in V10.

## Version Notes

### V1 — Baseline std::async

- Unbounded async branching with `maxParallelDepth = 8`.
- Minimal heuristics; thread creation overhead dominates on larger puzzles.

### V2 — Thread Control over Async

- Adds manual task limiting/queueing to reduce async explosion.
- Still uses `std::async` for parallel branches; no external dependencies.

### V3–V4 — Pruning and Ordering

- Earlier constraint checks and “most constrained edge first” ordering.
- Focused on shrinking the search tree before adding more concurrency.

### V5 — Hybrid Pruning + Parallel Search

- Combines pruning with controlled parallel branching.
- Introduces more state tracking (undecided edge counts) to guide choices.

### V6–V8 — Incremental Refinement

- Depth-limit tuning, heuristic cleanup, and propagation tweaks.
- Remain in the async family; useful as intermediate checkpoints.

### V9 — Final Async Polish

- Last iteration before the rewrite; code still includes `std::async` calls.
- Good reference for “best possible without TBB.”

### V10 — TBB Rewrite (Current)

- Introduces `tbb::task_group`, optional `tbb::task_arena`, and adaptive depth.
- Heuristics cleaned up with lambdas; this code became `main.cpp`.
- Requires building with `-DUSE_TBB` and linking against Intel TBB.

## Building Specific Versions

```bash
# Build V1 baseline (no TBB required)
g++ -std=c++17 -O3 tests/old_versions/v01_baseline.cpp -o slitherlink_v01

# Build V10 with TBB (macOS/Homebrew paths shown)
g++ -std=c++17 -O3 -DUSE_TBB \
  -I/opt/homebrew/include -L/opt/homebrew/lib -ltbb \
  tests/old_versions/v10_final.cpp -o slitherlink_v10
```

## Benchmark Guidance

⚠️ **Important:** Performance varies significantly based on puzzle characteristics (clue density, symmetry, forced moves).

**Expected Results (Current V10/main.cpp):**

- 4×4: ~0.001s (~100× faster than V1's ~0.1s)
- 5×5: ~0.001s (~500× faster than V1's ~0.5s)
- 8×8: ~0.5-1s, high variance (0.3s-11s observed) (~15-30× faster than V1's ~15s)
- 10×10: Most timeout (>60s), some challenging puzzles unsolvable

**Benchmarking Best Practices:**

- Keep async-era results (V1–V9) separate from TBB-era results (V10/main)
- Record puzzle size, clue density, and thread count for every run
- Test multiple puzzles of each size to capture variance
- Re-run benchmarks after any heuristic or depth limit changes
- Use scripts/test_solver.sh for consistent automated testing

## How to Navigate the History

1. Start with `v01_baseline.cpp` to understand the basic solver and data structures.
2. Compare `v02_threadpool.cpp` to see manual thread limiting layered on async.
3. Skim `v04_from_history.cpp` and `v05_from_history.cpp` to follow pruning improvements.
4. Use `v09_from_history.cpp` as the “last async” reference point.
5. Study `v10_final.cpp` (and `main.cpp`) for the TBB-based production code.

## Key Takeaways

- The async approach delivered incremental gains but hit scaling limits.
- The real architectural shift happened once at V10 with TBB.
- Documentation that suggested gradual TBB adoption across V2–V9 was aspirational; the repository code shows a single TBB jump.

This document is now consistent with the checked-in sources and should be used as the canonical reference for version history.
