# Performance Suggestions for Slitherlink Solver

## 1) Backtracking with Undo Stack
- **Issue**: Each branch clones the entire `State`, which is heavy (edge states, counters, queues).
- **Idea**: Mutate `State` in place and record changes (edge value, point/cell counters, queue inserts). After exploring a branch, roll back the recorded diffs.
- **Benefit**: Dramatically cuts allocations and memory bandwidth in `search`, usually yielding the biggest single speedup.

## 2) Reuse Scratch Buffers in Propagation
- **Issue**: `propagateConstraints` allocates `cellQueue`, `pointQueue`, and their visited flags on every call.
- **Idea**: Store these buffers on the solver (or per depth) and clear/resize them instead of reallocating.
- **Benefit**: Reduces churn in the hot loop; helps CPU cache locality and allocator pressure.

## 3) Dirty-Set Validation Instead of Full Scans
- **Issue**: `quickValidityCheck` and `propagateConstraints` often scan all points/clue cells every recursion.
- **Idea**: Maintain “dirty” queues of cells/points touched by the latest edge decisions and only re-check those. Seed initial dirties from the first decisions.
- **Benefit**: Turns repeated O(N) checks into O(changes), which matters on large grids.

## 4) Avoid Rebuilding the Graph
- **Issue**: `Solver::buildEdges` rebuilds the edge lists and indices even though `GraphBuilder` already does this for the printer.
- **Idea**: Build the graph once (via `GraphBuilder`) and pass its outputs into `Solver`, or share a common graph object.
- **Benefit**: Saves setup time, especially when solving multiple puzzles in one run.

## 5) Faster Edge Selection Bookkeeping
- **Issue**: `selectNextEdge` scores every undecided edge each node (O(E) per step).
- **Idea**: Keep a priority bucket/queue keyed by the heuristic score and update only edges affected by recent decisions. Cheap rescoring can live alongside a small “touched edges” list.
- **Benefit**: Cuts heuristic overhead; more time goes into actual search/pruning.

## 6) Cheaper Final Validation
- **Issue**: `finalCheckAndStore` rebuilds adjacency and DFSes from scratch for every candidate solution.
- **Idea**: Preallocate adjacency buffers or maintain incremental degree/adj info; short-circuit as soon as a vertex degree is not {0,2}. Reuse buffers for the DFS/stack.
- **Benefit**: Less time spent verifying, more time branching; important when finding all solutions.

## 7) Right-Size Parallelism
- **Issue**: Early parallel branching or tiny puzzles can suffer from thread/task overhead.
- **Idea**: Gate parallelism on puzzle size and branching factor; raise/lower `maxParallelDepth` dynamically and skip threading for small grids.
- **Benefit**: Keeps threads for cases that pay off; lowers overhead on easy/medium puzzles.

## 8) Data Layout / Cache Wins
- **Issue**: Some vectors hold `int` where smaller types would suffice, and layout is not contiguous for hot reads.
- **Idea**: Use `uint8_t`/`int16_t` where counts fit; ensure edge/cell/point arrays are contiguous and padded to avoid false sharing in parallel paths.
- **Benefit**: Better cache residency and fewer bytes moved per operation.

## 9) I/O Path
- **Issue**: Repeatedly printing during search (especially under TBB) can serialize progress.
- **Idea**: Defer detailed printing; collect solutions and print summaries or only print on final results.
- **Benefit**: Removes I/O from the hot path; smoother parallel execution.

## 10) Profiling Guidance
- **Issue**: Without measurements, optimizations may miss the real hotspots.
- **Idea**: Use `perf`/`Instruments`/`vtune` (depending on platform) on representative puzzles; focus changes where time is actually spent.
- **Benefit**: Higher ROI and less code churn; verifies each optimization step.
