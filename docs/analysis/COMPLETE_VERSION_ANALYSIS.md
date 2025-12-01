# Complete Version Analysis (Source of Truth)

This document reconciles the version history with the actual code in the repository. The authoritative files live in `tests/old_versions/` (mirrored in `BackUp_oldCode/old_versions/`). Versions **1–9** are std::async based; **Version 10** (current `main.cpp`) is the first Intel TBB rewrite.

## Where Each Version Lives

- `tests/old_versions/version.txt`: Narrative snippets for V1–V9 (all std::async).
- `tests/old_versions/v01_baseline.cpp` … `v09_from_history.cpp`: Concrete code for V1–V9 (still std::async).
- `tests/old_versions/v10_final.cpp` and `main.cpp`: TBB rewrite (current production).

## Timeline at a Glance

| Version | Concurrency Model            | Theme                              | File(s)                                               |
| ------- | --------------------------- | ---------------------------------- | ----------------------------------------------------- |
| V1      | std::async (unbounded)      | Baseline solver                    | `v01_baseline.cpp`                                   |
| V2      | std::async + thread limits  | Manual thread/queue experiment     | `v02_threadpool.cpp`                                 |
| V3      | std::async                  | Pruning/branch ordering tweaks     | `v03_from_history.cpp`                               |
| V4      | std::async                  | Smarter pruning & forced moves     | `v04_from_history.cpp`                               |
| V5      | std::async                  | Hybrid pruning + parallel search   | `v05_from_history.cpp`                               |
| V6      | (not stored separately)     | Iterative refinement (async era)   | covered by V5/V7 notes                                |
| V7      | std::async                  | Depth tuning & heuristics cleanup  | `v07_from_history.cpp` (comments mention TBB, code does not) |
| V8      | (not stored separately)     | Minor cleanup in async era         | covered by V7/V9 notes                                |
| V9      | std::async                  | Final async polish                 | `v09_from_history.cpp`                               |
| V10     | Intel TBB                   | Task-based rewrite (current)       | `v10_final.cpp`, `main.cpp`                          |

**Key fact:** No TBB appears in any V1–V9 source file; all TBB code starts at V10.

## Version Details

### V1 – Baseline std::async
- Unbounded `std::async` backtracking with `maxParallelDepth = 8`.
- Minimal heuristics; high thread creation overhead.

### V2 – Thread Control Experiment
- Adds a manual task queue and thread limits to reduce async explosion.
- Still uses `std::async` for branches; no TBB.

### V3–V5 – Pruning and Ordering
- Introduce better edge ordering, early constraint checks, and forced-move propagation.
- Remain `std::async`-only; improvements are algorithmic, not architectural.

### V6–V8 – Incremental Async Refinement
- Iterative tuning of heuristics, depth limits, and propagation.
- Some file headers reference TBB conceptually, but the code continues to use `std::async`.

### V9 – Final Async Polish
- Last std::async-based version. Still no TBB includes or macros.
- Goal was to squeeze remaining performance before a larger rewrite.

### V10 – Intel TBB Rewrite (Current)
- First and only version using TBB (`tbb::task_group`, `tbb::task_arena`, etc.).
- Lives in `tests/old_versions/v10_final.cpp` and the repository `main.cpp`.
- Increases parallel depth, adds adaptive depth calculation, and cleans heuristics with lambdas.

## Practical Notes

- If you need a std::async baseline for comparison, use `v09_from_history.cpp`.
- If you need the production implementation, use `main.cpp` (TBB-enabled).
- Any documentation that claimed gradual TBB adoption across V2–V9 was aspirational; the code history shows a single TBB jump at V10.

## Benchmark Caveats

Benchmark figures in older notes varied and sometimes mixed async/TBB eras. When running new benchmarks:
- Build specific versions directly from `tests/old_versions/`.
- Document puzzle size, clue density, and thread count to keep comparisons fair.

This aligned summary removes the previous conflicts and matches the actual source files.
