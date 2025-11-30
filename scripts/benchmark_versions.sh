#!/bin/bash
# Benchmark all compiled versions against a puzzle
# Usage: ./benchmark_versions.sh <puzzle_file>

if [ $# -lt 1 ]; then
    echo "Usage: $0 <puzzle_file>"
    echo "Example: $0 example8x8.txt"
    exit 1
fi

PUZZLE=$1

if [ ! -f "$PUZZLE" ]; then
    echo "Error: Puzzle file '$PUZZLE' not found"
    exit 1
fi

echo "📊 Benchmarking Slitherlink Versions"
echo "Puzzle: $PUZZLE"
echo "====================================="
echo ""

# Read puzzle size
read n m < "$PUZZLE"
echo "Puzzle size: ${n}×${m}"
echo ""

# Function to run and time a version
benchmark_version() {
    local ver=$1
    local exe="./slitherlink_v${ver}"
    
    if [ ! -f "$exe" ]; then
        echo "Version $ver: ⚠️  Not compiled"
        return
    fi
    
    echo -n "Version $ver: "
    
    # Run with timeout (30s for small, 120s for large)
    local timeout=30
    if [ $n -ge 8 ]; then
        timeout=120
    fi
    
    # Time the execution
    local start=$(date +%s.%N)
    timeout $timeout $exe "$PUZZLE" > /tmp/slitherlink_v${ver}.out 2>&1
    local exit_code=$?
    local end=$(date +%s.%N)
    
    if [ $exit_code -eq 124 ]; then
        echo "⏱️  TIMEOUT (>${timeout}s)"
    elif [ $exit_code -ne 0 ]; then
        echo "❌ ERROR (exit code $exit_code)"
    else
        local elapsed=$(echo "$end - $start" | bc)
        local solutions=$(grep "Found" /tmp/slitherlink_v${ver}.out | awk '{print $2}')
        echo "✅ ${elapsed}s (${solutions} solution)"
    fi
}

# Benchmark all versions
for v in 01 02 03 04 05 06 08 09 10; do
    benchmark_version $v
done

echo ""
echo "====================================="
echo "📈 Performance Comparison Complete"
echo ""
echo "Expected improvements for 8×8 puzzle:"
echo "  V1  → V2:  3× faster (TBB)"
echo "  V2  → V4:  8× faster (Adaptive depth)"
echo "  V4  → V6:  1.3× faster (Heuristics + propagation)"
echo "  V6  → V10: Same/slight variance"
echo "  V1  → V10: ~21× faster overall"

# Cleanup
rm -f /tmp/slitherlink_v*.out
