#!/bin/bash
# Benchmark script for Slitherlink solver

SOLVER="./cmake-build-debug/slitherlink"
PUZZLES_DIR="puzzles/examples"
RESULTS_FILE="benchmark_results.txt"

echo "=== Slitherlink Solver Benchmarks ===" | tee "$RESULTS_FILE"
echo "Date: $(date)" | tee -a "$RESULTS_FILE"
echo "System: $(uname -m) $(uname -s)" | tee -a "$RESULTS_FILE"
echo "" | tee -a "$RESULTS_FILE"

# Function to run benchmark
run_benchmark() {
    local puzzle=$1
    local threads=$2
    local description=$3
    
    echo "[$description] Testing $puzzle with $threads threads..." | tee -a "$RESULTS_FILE"
    
    # Run 3 times and take average
    total_time=0
    for i in 1 2 3; do
        if [ "$threads" == "auto" ]; then
            output=$($SOLVER "$PUZZLES_DIR/$puzzle" 2>&1)
        else
            output=$($SOLVER "$PUZZLES_DIR/$puzzle" --threads "$threads" 2>&1)
        fi
        
        time=$(echo "$output" | grep "Time:" | awk '{print $2}')
        total_time=$(echo "$total_time + $time" | bc)
    done
    
    avg_time=$(echo "scale=6; $total_time / 3" | bc)
    echo "  Average time: ${avg_time}s" | tee -a "$RESULTS_FILE"
    echo "" | tee -a "$RESULTS_FILE"
}

# Benchmark different puzzle sizes
echo "=== Puzzle Size Scaling ===" | tee -a "$RESULTS_FILE"
run_benchmark "example4x4_easy.txt" "auto" "4x4 Easy"
run_benchmark "example5x5_medium.txt" "auto" "5x5 Medium"
run_benchmark "example6x6_medium.txt" "auto" "6x6 Medium"

# Benchmark thread scaling on medium puzzle
echo "=== Thread Scaling (5x5 puzzle) ===" | tee -a "$RESULTS_FILE"
for threads in 1 2 4 8; do
    run_benchmark "example5x5_medium.txt" "$threads" "5x5 with $threads threads"
done

# Benchmark CPU percentage
echo "=== CPU Usage (5x5 puzzle) ===" | tee -a "$RESULTS_FILE"
for cpu in 0.25 0.5 0.75 1.0; do
    threads=$(echo "scale=0; $(nproc 2>/dev/null || sysctl -n hw.ncpu) * $cpu / 1" | bc)
    [ "$threads" -lt 1 ] && threads=1
    echo "[CPU ${cpu}x] Testing with $threads threads..." | tee -a "$RESULTS_FILE"
    $SOLVER "$PUZZLES_DIR/example5x5_medium.txt" --threads "$threads" 2>&1 | grep "Time:" | tee -a "$RESULTS_FILE"
    echo "" | tee -a "$RESULTS_FILE"
done

echo "=== Benchmark Complete ===" | tee -a "$RESULTS_FILE"
echo "Results saved to: $RESULTS_FILE"
