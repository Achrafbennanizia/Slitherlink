#!/bin/bash
# Slitherlink Architecture Migration Script
# Date: December 8, 2025
# Purpose: Automate the restructuring of the project

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "=========================================="
echo "Slitherlink Architecture Migration"
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored messages
print_step() {
    echo -e "${GREEN}[STEP]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Change to project root
cd "$PROJECT_ROOT"

# Step 1: Create backup
print_step "Creating backup branch..."
if git rev-parse --verify backup-before-restructure >/dev/null 2>&1; then
    print_warning "Backup branch already exists, skipping..."
else
    git branch backup-before-restructure
    git tag v1.0-before-restructure
    echo "✓ Backup created: backup-before-restructure"
fi

# Step 2: Create new directory structure
print_step "Creating new directory structure..."

mkdir -p docs/{user,developer,technical,history/archived}
mkdir -p src/slitherlink/{core,solver,io,factory,utils}
mkdir -p apps/slitherlink_cli
mkdir -p tests/{unit,integration,benchmarks}
mkdir -p puzzles/samples/{4x4,6x6,8x8,10x10,15x15,20x20}
mkdir -p .github/workflows

echo "✓ Directory structure created"

# Step 3: Move documentation files
print_step "Consolidating documentation..."

# User Documentation
[ -f USER_GUIDE.md ] && mv USER_GUIDE.md docs/user/
[ -f QUICK_REFERENCE.md ] && mv QUICK_REFERENCE.md docs/user/QUICK_START.md

# Developer Documentation
[ -f SOLID_ARCHITECTURE.md ] && mv SOLID_ARCHITECTURE.md docs/developer/
[ -f SOLID_PRINCIPLES.md ] && mv SOLID_PRINCIPLES.md docs/developer/
[ -f ARCHITECTURE.md ] && mv ARCHITECTURE.md docs/developer/
[ -f CODE_STRUCTURE.md ] && mv CODE_STRUCTURE.md docs/developer/
[ -f README_SOLID.md ] && mv README_SOLID.md docs/developer/

# Technical Documentation
[ -f REPORT.md ] && mv REPORT.md docs/technical/OPTIMIZATION_REPORT.md
[ -f OPTIMIZATION_COMPARISON.md ] && mv OPTIMIZATION_COMPARISON.md docs/technical/
[ -f PERFORMANCE_OPTIMIZATION.md ] && mv PERFORMANCE_OPTIMIZATION.md docs/technical/PERFORMANCE_ANALYSIS.md
[ -f PHASE2_OPTIMIZATIONS.md ] && mv PHASE2_OPTIMIZATIONS.md docs/technical/
[ -f PHASE3_OPTIMIZATIONS.md ] && mv PHASE3_OPTIMIZATIONS.md docs/technical/

# History
[ -f REFACTORING_CHANGELOG.md ] && mv REFACTORING_CHANGELOG.md docs/history/
[ -f COMPLETE_CODE_HISTORY.md ] && mv COMPLETE_CODE_HISTORY.md docs/history/archived/
[ -f COMPLETE_CHAT_HISTORY.txt ] && mv COMPLETE_CHAT_HISTORY.txt docs/history/archived/
[ -f CHAT_SESSION_SUMMARY.txt ] && mv CHAT_SESSION_SUMMARY.txt docs/history/archived/

# Archive Redundant
[ -f ARCHITECTURE_IMPROVEMENTS.md ] && mv ARCHITECTURE_IMPROVEMENTS.md docs/history/archived/
[ -f PROJECT_ARCHITECTURE_COMPLETE.md ] && mv PROJECT_ARCHITECTURE_COMPLETE.md docs/history/archived/
[ -f PROJECT_COMPLETION_SUMMARY.md ] && mv PROJECT_COMPLETION_SUMMARY.md docs/history/archived/
[ -f FINAL_OPTIMIZATION_SUMMARY.md ] && mv FINAL_OPTIMIZATION_SUMMARY.md docs/history/archived/
[ -f OPTIMIZATION_RESULTS.md ] && mv OPTIMIZATION_RESULTS.md docs/history/archived/
[ -f PERFORMANCE_SUGGESTIONS.md ] && mv PERFORMANCE_SUGGESTIONS.md docs/history/archived/

echo "✓ Documentation organized"

# Step 4: Remove duplicate headers (keep only slitherlink namespace)
print_step "Removing duplicate headers..."

if [ -d "include/slitherlink" ]; then
    # Only remove top-level headers if slitherlink directory exists
    for header in Edge.h Grid.h IHeuristic.h IPropagator.h IValidator.h \
                  OptimizedPropagator.h SmartHeuristic.h Solution.h \
                  Solver.h StandardValidator.h State.h; do
        if [ -f "include/$header" ]; then
            rm "include/$header"
            echo "  Removed include/$header"
        fi
    done
    echo "✓ Duplicate headers removed"
else
    print_warning "Slitherlink namespace directory not found, skipping header cleanup"
fi

# Step 5: Move source files (only if not already organized)
print_step "Reorganizing source files..."

# Move core implementations
[ -f src/Grid.cpp ] && [ ! -f src/slitherlink/core/Grid.cpp ] && mv src/Grid.cpp src/slitherlink/core/
[ -f src/State.cpp ] && [ ! -f src/slitherlink/core/State.cpp ] && mv src/State.cpp src/slitherlink/core/

# Move solver implementations
[ -f src/Solver.cpp ] && [ ! -f src/slitherlink/solver/Solver.cpp ] && mv src/Solver.cpp src/slitherlink/solver/
[ -f src/OptimizedPropagator.cpp ] && [ ! -f src/slitherlink/solver/OptimizedPropagator.cpp ] && mv src/OptimizedPropagator.cpp src/slitherlink/solver/
[ -f src/SmartHeuristic.cpp ] && [ ! -f src/slitherlink/solver/SmartHeuristic.cpp ] && mv src/SmartHeuristic.cpp src/slitherlink/solver/
[ -f src/StandardValidator.cpp ] && [ ! -f src/slitherlink/solver/StandardValidator.cpp ] && mv src/StandardValidator.cpp src/slitherlink/solver/

# Move existing organized files if needed
if [ -d src/core ] && [ -f src/core/Config.cpp ]; then
    [ ! -f src/slitherlink/utils/Config.cpp ] && mv src/core/Config.cpp src/slitherlink/utils/
    rmdir src/core 2>/dev/null || true
fi

if [ -d src/io ] && [ ! -d src/slitherlink/io/GridReader.cpp ]; then
    cp -r src/io/* src/slitherlink/io/ 2>/dev/null || true
fi

if [ -d src/factory ] && [ ! -d src/slitherlink/factory/SolverFactory.cpp ]; then
    cp -r src/factory/* src/slitherlink/factory/ 2>/dev/null || true
fi

echo "✓ Source files reorganized"

# Step 6: Move application files
print_step "Organizing application files..."

[ -f main.cpp ] && [ ! -f apps/slitherlink_cli/main.cpp ] && mv main.cpp apps/slitherlink_cli/
[ -f main_solid.cpp ] && mv main_solid.cpp apps/slitherlink_cli/main_solid.cpp.backup

echo "✓ Application files organized"

# Step 7: Reorganize tests
print_step "Reorganizing tests..."

if [ -d tests ]; then
    [ -f tests/test_grid.cpp ] && [ ! -f tests/unit/test_grid.cpp ] && mv tests/test_grid.cpp tests/unit/
    [ -f tests/test_solver_basic.cpp ] && [ ! -f tests/unit/test_solver_basic.cpp ] && mv tests/test_solver_basic.cpp tests/unit/
fi

# Move benchmarks
if [ -d benchmarks ]; then
    cp -r benchmarks/* tests/benchmarks/ 2>/dev/null || true
fi

echo "✓ Tests reorganized"

# Step 8: Organize puzzles
print_step "Organizing puzzle files..."

if [ -d puzzles/examples ]; then
    # Move by size
    mv puzzles/examples/example4x4*.txt puzzles/samples/4x4/ 2>/dev/null || true
    mv puzzles/examples/example6x6*.txt puzzles/samples/6x6/ 2>/dev/null || true
    mv puzzles/examples/example8x8*.txt puzzles/samples/8x8/ 2>/dev/null || true
    mv puzzles/examples/example10x10*.txt puzzles/samples/10x10/ 2>/dev/null || true
    mv puzzles/examples/example15x15*.txt puzzles/samples/15x15/ 2>/dev/null || true
    mv puzzles/examples/example20x20*.txt puzzles/samples/20x20/ 2>/dev/null || true
    
    # Move remaining files
    mv puzzles/examples/*.txt puzzles/samples/ 2>/dev/null || true
    
    # Remove empty examples directory
    rmdir puzzles/examples 2>/dev/null || true
fi

echo "✓ Puzzles organized"

# Step 9: Clean build artifacts from git
print_step "Cleaning build artifacts..."

# Update .gitignore if needed
if ! grep -q "cmake-build-" .gitignore 2>/dev/null; then
    cat >> .gitignore << 'EOF'

# Build directories
build/
cmake-build-*/
.cmake/

# Binaries
test_v11
*.exe
EOF
    echo "✓ Updated .gitignore"
fi

# Remove from git cache (but keep files locally)
git rm -r --cached cmake-build-* 2>/dev/null || true
git rm -r --cached build/ 2>/dev/null || true
git rm --cached test_v11 2>/dev/null || true

echo "✓ Build artifacts cleaned"

# Step 10: Create documentation index
print_step "Creating documentation index..."

cat > docs/README.md << 'EOF'
# Slitherlink Documentation

Welcome to the Slitherlink Solver documentation!

## 📚 Documentation Index

### For Users
- **[User Guide](user/USER_GUIDE.md)** - Complete guide with examples and tutorials
- **[Quick Start](user/QUICK_START.md)** - Get started in 5 minutes
- **[FAQ](user/FAQ.md)** - Frequently asked questions *(coming soon)*

### For Developers
- **[Architecture Overview](developer/ARCHITECTURE.md)** - System design and structure
- **[SOLID Principles](developer/SOLID_PRINCIPLES.md)** - Design patterns used
- **[Contributing Guide](developer/CONTRIBUTING.md)** - How to contribute *(coming soon)*

### Technical Documentation
- **[Optimization Report](technical/OPTIMIZATION_REPORT.md)** - Detailed performance analysis
- **[Performance Analysis](technical/PERFORMANCE_ANALYSIS.md)** - Benchmarks and profiling
- **[Phase 2 Optimizations](technical/PHASE2_OPTIMIZATIONS.md)** - Advanced optimizations
- **[Phase 3 Optimizations](technical/PHASE3_OPTIMIZATIONS.md)** - Final improvements
- **[Optimization Comparison](technical/OPTIMIZATION_COMPARISON.md)** - Before/after comparison

### Project History
- **[Changelog](history/CHANGELOG.md)** - Version history *(coming soon)*
- **[Refactoring Log](history/REFACTORING_CHANGELOG.md)** - Architecture changes
- **[Archived Documentation](history/archived/)** - Historical documents

## 🎯 Quick Links

- **Main README**: [../README.md](../README.md)
- **Source Code**: [../src/](../src/)
- **Examples**: [../examples/](../examples/)
- **Tests**: [../tests/](../tests/)

## 📖 Getting Help

1. Check the [User Guide](user/USER_GUIDE.md) for usage examples
2. Review [Technical Documentation](technical/) for implementation details
3. Open an issue on GitHub for questions or bug reports

---

Last updated: December 8, 2025
EOF

echo "✓ Documentation index created"

# Step 11: Summary
print_step "Migration complete!"
echo ""
echo "=========================================="
echo "Summary of Changes"
echo "=========================================="
echo "✓ Documentation organized into docs/"
echo "✓ Duplicate headers removed"
echo "✓ Source files reorganized by namespace"
echo "✓ Applications separated to apps/"
echo "✓ Tests reorganized into unit/integration"
echo "✓ Puzzles organized by size"
echo "✓ Build artifacts cleaned"
echo "✓ Documentation index created"
echo ""
echo "=========================================="
echo "Next Steps"
echo "=========================================="
echo "1. Review changes: git status"
echo "2. Test build: mkdir build && cd build && cmake .. && make"
echo "3. Run tests: ctest"
echo "4. Commit changes: git add . && git commit -m 'refactor: Restructure project architecture'"
echo ""
echo "To rollback: git checkout backup-before-restructure"
echo ""

print_warning "IMPORTANT: You may need to update CMakeLists.txt and include paths manually!"

exit 0
