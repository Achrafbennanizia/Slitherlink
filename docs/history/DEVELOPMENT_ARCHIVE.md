# Development History & Version Archive

> Archived overview. For current layout/entrypoints, see `README.md` and `docs/developer/ARCHITECTURE.md`.

This section provides links to detailed development documentation and historical code versions.

## 📚 Documentation Resources

### Complete Development Journey

- **[CONVERSATION_HISTORY.md](versions/CONVERSATION_HISTORY.md)** - Complete conversation logs and development decisions
  - All chat conversations from the 4-week development period
  - Detailed explanations of every decision and experiment
  - Failed experiment analysis with code snippets
  - Performance discussions and profiling results
  - 50+ pages of comprehensive development narrative

### Version Archive

- **[VERSION_HISTORY.md](versions/VERSION_HISTORY.md)** - Overview of all versions
  - Quick reference table with performance metrics
  - Compilation instructions for each version
  - Key milestones and breakthroughs
  - Failed experiment summaries

## 📦 Code Versions Available

All historical versions are preserved in the `versions/` directory:

| Version | File                       | Date               | Performance (8×8) | Description                         |
| ------- | -------------------------- | ------------------ | ----------------- | ----------------------------------- |
| **V1**  | `v01_baseline.cpp`         | Week 1, Days 1-3   | 15.0s             | Initial backtracking implementation |
| **V2**  | `v02_tbb_basic.cpp`        | Week 1, Days 5-7   | 5.0s              | TBB parallelization added           |
| **V3**  | `v03_cpu_limit.cpp`        | Week 2, Days 8-9   | 6.0s              | CPU limiting to 50%                 |
| **V4**  | `v04_dynamic_depth.cpp`    | Week 2, Days 10-11 | 0.64s             | **Adaptive depth breakthrough**     |
| **V5**  | `v05_smart_heuristics.cpp` | Week 2, Days 12-14 | 0.53s             | Intelligent edge selection          |
| **V6**  | `v06_propagation.cpp`      | Week 3, Days 15-17 | 0.48s             | Constraint propagation              |
| **V7**  | `v07_ortools_failed.cpp`   | Week 3, Days 18-21 | N/A               | OR-Tools attempt (FAILED)           |
| **V8**  | `v08_cleanup.cpp`          | Week 3, Days 22-24 | 0.48s             | OR-Tools removed, cleanup           |
| **V9**  | `v09_tbb_validation.cpp`   | Week 4, Days 25-27 | 0.70s             | TBB in validation                   |
| **V10** | `v10_final.cpp`            | Week 4, Day 28     | 0.705s            | **Production version**              |

### Current Production Version

The current `main.cpp` is identical to `v10_final.cpp` (987 lines, fully optimized).

## 🔬 Failed Experiments (Documented)

These experiments didn't make it into production but provided valuable insights:

1. **OpenMP Attempt** (Day 4)

   - Status: Abandoned
   - Issue: Data parallelism unsuitable for search trees
   - Learning: Use task-based parallelism (TBB)

2. **Simple sqrt Depth** (Day 10 AM)

   - Status: Reverted
   - Issue: All depths too shallow, killed performance
   - Learning: Need empirical tuning

3. **Density-Only Depth** (Day 10 PM)

   - Status: Enhanced into V4
   - Issue: Ignored puzzle size
   - Learning: Combine multiple factors

4. **OR-Tools Integration** (Days 18-21) - See `v07_ortools_failed.cpp`
   - Status: Completely removed
   - Issue: 3 different constraint approaches all failed
   - Learning: Generic CP-SAT unsuitable for graph topology
   - **Most detailed documentation** in CONVERSATION_HISTORY.md

## 🎯 How to Use Historical Versions

### Compile Any Version

```bash
# Basic compilation (no TBB)
g++ -std=c++17 -O3 versions/v01_baseline.cpp -o slitherlink_v01

# With TBB (V2 and later)
g++ -std=c++17 -O3 -DUSE_TBB \
    -I/opt/intel/oneapi/tbb/latest/include \
    -L/opt/intel/oneapi/tbb/latest/lib -ltbb \
    versions/v10_final.cpp -o slitherlink_v10

# On macOS with Homebrew TBB
g++ -std=c++17 -O3 -DUSE_TBB \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib -ltbb \
    versions/v10_final.cpp -o slitherlink_v10
```

### Benchmark Comparison

```bash
# Run all versions on same puzzle
for v in 01 02 04 05 06 08 09 10; do
    echo "=== Version $v ==="
    time ./slitherlink_v$v example8x8.txt
done
```

### Performance Evolution Demo

```bash
# Compare V1 (baseline) vs V10 (final)
echo "Version 1 (Baseline):"
time ./slitherlink_v01 example4x4.txt

echo "\nVersion 10 (Final):"
time ./slitherlink_v10 example4x4.txt

# Expected: 50× speedup (0.100s → 0.002s)
```

## 📊 Development Statistics

### Timeline

- **Duration**: 4 weeks (28 days)
- **Productive days**: 21 days (75%)
- **Exploration/failures**: 7 days (25%)

### Code Evolution

- **Starting size**: 800 lines
- **Peak size**: 1630 lines (with OR-Tools)
- **Final size**: 987 lines
- **Net change**: +23% code, +2100% performance

### Performance Journey

```
Version  1: 15.00s (8×8) → Baseline
Version  2:  5.00s (8×8) → 3× faster
Version  4:  0.64s (8×8) → 10× faster  ⭐ Biggest win
Version  6:  0.48s (8×8) → 1.3× faster
Version 10:  0.70s (8×8) → 21× total   ✅ Production
```

### Lessons Learned

1. **25% failure rate is normal** - Exploration is valuable
2. **Profile before optimizing** - Found 15% validation bottleneck
3. **Domain knowledge > generic tools** - OR-Tools couldn't compete
4. **Incremental wins compound** - Many small improvements = big gain
5. **Document failures** - Learn from what didn't work

## 🔍 Deep Dive Resources

### For Understanding Algorithms

Start with main README.md sections:

- Architecture (data structures)
- Algorithms (backtracking, propagation, heuristics)
- Code Structure (method explanations)

### For Understanding Optimization Journey

Read in this order:

1. `VERSION_HISTORY.md` - Quick overview
2. Main README "Performance Optimization Journey" section
3. `CONVERSATION_HISTORY.md` - Complete details

### For Learning from Failures

See `CONVERSATION_HISTORY.md` sections:

- "Day 4: OpenMP Experiment"
- "Day 10: Adaptive Depth Failures"
- "Days 18-21: OR-Tools Disaster" (most detailed)

### For Code Comparison

```bash
# Compare two versions
diff -u versions/v01_baseline.cpp versions/v10_final.cpp | less

# See what changed in V4 (biggest optimization)
diff -u versions/v03_cpu_limit.cpp versions/v04_dynamic_depth.cpp

# See OR-Tools failure
cat versions/v07_ortools_failed.cpp | grep "ortools"
```

## 💡 Key Insights for Future Projects

### What Worked

1. ✅ **Task-based parallelism (TBB)** - Perfect for search trees
2. ✅ **Adaptive strategies** - Different puzzles need different approaches
3. ✅ **Domain heuristics** - Problem-specific knowledge helps
4. ✅ **Incremental optimization** - Small testable steps
5. ✅ **Profiling-driven** - Measure before optimizing

### What Didn't Work

1. ❌ **Data parallelism (OpenMP)** - Wrong model for search
2. ❌ **Fixed strategies** - One-size-fits-all fails
3. ❌ **Generic solvers (OR-Tools)** - Can't beat specialized algorithms
4. ❌ **Premature optimization** - Need profiling data first
5. ❌ **Complex over simple** - Simpler solutions often better

### Process Recommendations

1. **Keep baselines** - Honest performance comparison
2. **Version everything** - Easy rollback on failures
3. **Document failures** - Learning opportunity
4. **Profile regularly** - Find real bottlenecks
5. **Test incrementally** - Catch regressions early

## 📝 Citation

If you use this code or learn from this development journey, please reference:

```
Slitherlink Solver - High-Performance Parallel Implementation
Development: November 2025
Repository: github.com/Achrafbennanizia/Slitherlink
Documentation: Complete conversation history and 10 major versions
Key Achievement: 21-50× performance improvement through iterative optimization
```

## 🤝 Contributing

This project serves as an educational resource showing:

- Real development journey with failures and successes
- How to optimize complex algorithms
- Task-based parallelism with Intel TBB
- Importance of profiling and measurement

Feel free to:

- Study the version progression
- Learn from documented failures
- Use as a reference for similar projects
- Extend with new optimizations (see "Lessons for Future Work" in main README)

---

**Note**: All code versions are preserved exactly as they were during development, including failed experiments. This provides an honest view of the iterative development process.
