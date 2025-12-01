#!/bin/bash

# Comprehensive Slitherlink Puzzle Benchmark Suite
# Tests all puzzle sizes with all difficulty levels

SOLVER="./cmake-build-debug/slitherlink"
PUZZLE_DIR="puzzles/examples"
TIMEOUT_CMD=""
TIMEOUT=60  # 1 minute max per puzzle

# Check for timeout command (gtimeout on macOS, timeout on Linux)
if command -v gtimeout &> /dev/null; then
    TIMEOUT_CMD="gtimeout $TIMEOUT"
elif command -v timeout &> /dev/null; then
    TIMEOUT_CMD="timeout $TIMEOUT"
else
    echo "Warning: No timeout command found. Tests will run without time limits."
    TIMEOUT_CMD=""
fi

RESULTS_FILE="benchmark_results_comprehensive.txt"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "╔════════════════════════════════════════════════════════════╗"
echo "║   COMPREHENSIVE SLITHERLINK BENCHMARK SUITE                ║"
echo "║   Testing all sizes (4×4 to 20×20) × all difficulties     ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Initialize results file
echo "Comprehensive Slitherlink Benchmark Results" > "$RESULTS_FILE"
echo "Generated: $(date)" >> "$RESULTS_FILE"
echo "═══════════════════════════════════════════════════════════════" >> "$RESULTS_FILE"
echo "" >> "$RESULTS_FILE"

# Define test files organized by size and difficulty
TEST_SIZES=("4x4" "5x5" "6x6" "7x7" "8x8" "10x10" "12x12" "15x15" "20x20")
TEST_DIFFICULTIES=("easy" "medium" "hard" "extreme" "original" "simple" "dense" "box")

# Counters
total_tests=0
passed_tests=0
failed_tests=0
timeout_tests=0

# Function to run a single test
run_test() {
    local name=$1
    local file=$2
    local filepath="$PUZZLE_DIR/$file"
    
    if [ ! -f "$filepath" ]; then
        echo -e "${YELLOW}SKIP${NC} $name - File not found"
        return
    fi
    
    total_tests=$((total_tests + 1))
    
    printf "Testing %-25s ... " "$name"
    
    # Run with timeout and capture timing
    local start_time=$(date +%s.%N)
    local output
    if [ -n "$TIMEOUT_CMD" ]; then
        output=$($TIMEOUT_CMD "$SOLVER" "$filepath" 2>&1)
        local exit_code=$?
    else
        output=$("$SOLVER" "$filepath" 2>&1)
        local exit_code=$?
    fi
    local end_time=$(date +%s.%N)
    local duration=$(echo "$end_time - $start_time" | bc)
    
    # Format duration
    local time_str
    if (( $(echo "$duration < 0.001" | bc -l) )); then
        time_str="<1ms"
    elif (( $(echo "$duration < 1" | bc -l) )); then
        time_str=$(printf "%.0fms" $(echo "$duration * 1000" | bc))
    elif (( $(echo "$duration < 60" | bc -l) )); then
        time_str=$(printf "%.2fs" "$duration")
    else
        local minutes=$(echo "$duration / 60" | bc)
        local seconds=$(echo "$duration % 60" | bc)
        time_str=$(printf "%dm %.1fs" "$minutes" "$seconds")
    fi
    
    # Analyze results
    if [ $exit_code -eq 124 ]; then
        echo -e "${RED}TIMEOUT${NC} (>${TIMEOUT}s)"
        timeout_tests=$((timeout_tests + 1))
        echo "$name: TIMEOUT (>${TIMEOUT}s)" >> "$RESULTS_FILE"
    elif echo "$output" | grep -q "Solution.*found"; then
        echo -e "${GREEN}PASS${NC} ($time_str)"
        passed_tests=$((passed_tests + 1))
        
        # Extract statistics
        local nodes=$(echo "$output" | grep -i "nodes" | grep -oE '[0-9,]+' | head -1 | tr -d ',')
        local solutions=$(echo "$output" | grep "Total solutions" | grep -oE '[0-9]+')
        
        echo "$name: SOLVED in $time_str (solutions: $solutions, nodes: $nodes)" >> "$RESULTS_FILE"
    elif echo "$output" | grep -q "No solution"; then
        echo -e "${BLUE}NO SOLUTION${NC} ($time_str)"
        echo "$name: NO SOLUTION in $time_str" >> "$RESULTS_FILE"
    else
        echo -e "${RED}FAIL${NC} ($time_str)"
        failed_tests=$((failed_tests + 1))
        echo "$name: FAILED in $time_str" >> "$RESULTS_FILE"
        echo "$output" | head -5 >> "$RESULTS_FILE"
    fi
}

# Run tests grouped by size
echo "Starting benchmark suite..."
echo ""

for size in "${TEST_SIZES[@]}"; do
    echo -e "${BLUE}═══ ${size} Puzzles ═══${NC}"
    echo "" >> "$RESULTS_FILE"
    echo "═══ ${size} Puzzles ═══" >> "$RESULTS_FILE"
    
    # Test all difficulty variants for this size
    for diff in "${TEST_DIFFICULTIES[@]}"; do
        filename="example${size}_${diff}.txt"
        filepath="$PUZZLE_DIR/$filename"
        
        if [ -f "$filepath" ]; then
            run_test "${size}_${diff}" "$filename"
        fi
    done
    
    # Also test the base file (exampleNxN.txt)
    filename="example${size}.txt"
    filepath="$PUZZLE_DIR/$filename"
    if [ -f "$filepath" ]; then
        run_test "${size}_original" "$filename"
    fi
    
    echo ""
done

# Summary
echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                    BENCHMARK SUMMARY                       ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Total tests:    $total_tests"
echo -e "Passed:         ${GREEN}$passed_tests${NC}"
echo -e "Failed:         ${RED}$failed_tests${NC}"
echo -e "Timeouts:       ${YELLOW}$timeout_tests${NC}"
echo ""
echo "Success rate:   $(echo "scale=1; $passed_tests * 100 / $total_tests" | bc)%"
echo ""
echo "Results saved to: $RESULTS_FILE"

# Summary to file
echo "" >> "$RESULTS_FILE"
echo "═══════════════════════════════════════════════════════════════" >> "$RESULTS_FILE"
echo "SUMMARY" >> "$RESULTS_FILE"
echo "═══════════════════════════════════════════════════════════════" >> "$RESULTS_FILE"
echo "Total tests:    $total_tests" >> "$RESULTS_FILE"
echo "Passed:         $passed_tests" >> "$RESULTS_FILE"
echo "Failed:         $failed_tests" >> "$RESULTS_FILE"
echo "Timeouts:       $timeout_tests" >> "$RESULTS_FILE"
echo "Success rate:   $(echo "scale=1; $passed_tests * 100 / $total_tests" | bc)%" >> "$RESULTS_FILE"
