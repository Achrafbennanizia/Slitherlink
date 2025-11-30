# Code Export Index

Use the paths below to browse or copy each solver snapshot.

## 📦 Code Files

### Current & Production Versions

- **Current solver (latest)**: `../main.cpp` (1023 lines, V10 production)
- **Final optimized version**: `v10_final.cpp` (987 lines, identical to main.cpp)
- **Baseline version**: `v01_baseline.cpp` (481 lines, no TBB)

### Historical Snapshots (from version.txt)

- `v01_from_history.cpp` - Week 1: Initial implementation
- `v02_from_history.cpp` - Week 1: TBB integration
- `v03_from_history.cpp` - Week 2: CPU limiting
- `v04_from_history.cpp` - Week 2: Adaptive depth ⭐
- `v05_from_history.cpp` - Week 2: Smart heuristics
- `v07_from_history.cpp` - Week 3: OR-Tools attempt (failed)
- `v09_from_history.cpp` - Week 4: TBB validation
- `v10_from_history.cpp` - Week 4: Lambda optimization

### Version Archive

- **Full text history**: `version.txt` (multi-version bundle)
- **Note**: version.txt did not contain standalone blocks for v06 or v08

## 📚 Documentation Files

### Code Analysis & Comparisons

- **CODE_EVOLUTION.md** ⭐ NEW! - Side-by-side code comparisons showing actual improvements
  - Data structures evolution (V1 → V10)
  - TBB integration details (V1 → V2)
  - Adaptive depth implementation (V3 → V4)
  - Edge selection optimization (V5)
  - Constraint propagation (V6)
  - Lambda optimization (V10)
  - Complete function evolution

### Development Documentation

- **VERSION_HISTORY.md** - Quick version overview with performance table
- **DEVELOPMENT_ARCHIVE.md** - Usage guide and learning resources
- **CONVERSATION_HISTORY.md** - Complete chat logs (1000+ lines)
- **README.md** - Archive navigation guide
- **NAVIGATION_GUIDE.md** - Complete documentation map

## 🛠️ Build & Test Tools

- **compile_all_versions.sh** - Automated compilation script
- **benchmark_versions.sh** - Performance comparison tool

## 🎯 Quick Links by Learning Goal

### Want to See Code Changes?

→ **CODE_EVOLUTION.md** - Real code with before/after comparisons

### Want Performance Data?

→ **VERSION_HISTORY.md** - Performance evolution table

### Want Complete Story?

→ **CONVERSATION_HISTORY.md** - All development decisions

### Want to Compile & Test?

→ Run `./compile_all_versions.sh` then `./benchmark_versions.sh ../example8x8.txt`

## 📊 File Statistics

| File                    | Lines | Purpose                  |
| ----------------------- | ----- | ------------------------ |
| CODE_EVOLUTION.md       | ~800  | Actual code comparisons  |
| CONVERSATION_HISTORY.md | ~1000 | Complete development log |
| VERSION_HISTORY.md      | ~200  | Quick reference          |
| DEVELOPMENT_ARCHIVE.md  | ~300  | Usage guide              |
| v01_baseline.cpp        | 481   | Initial code             |
| v10_final.cpp           | 987   | Final code               |
| ../main.cpp             | 1023  | Production code          |

## 💡 Tip

For the most detailed code understanding, read **CODE_EVOLUTION.md** which shows:

- Actual code from each version
- Line-by-line comparisons
- Why changes improved performance
- Complete function evolution

For the fastest overview, check **VERSION_HISTORY.md** performance table.
