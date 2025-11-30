#!/bin/bash

# Slitherlink Comprehensive Benchmark Suite
# Tests puzzles across different sizes and difficulty levels
# Outputs detailed timing and statistics

set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
SOLVER="./cmake-build-debug/slitherlink"
RESULTS_FILE="benchmark_results.csv"
DETAILED_LOG="benchmark_detailed.log"

# Check if solver exists
if [ ! -f "$SOLVER" ]; then
    echo -e "${RED}Error: Solver not found at $SOLVER${NC}"
    echo "Please build the project first: cmake --build cmake-build-debug"
    exit 1
fi

# Create results CSV header
echo "Puzzle,Size,Difficulty,Clues,Density,Time(s),Solutions,Status" > "$RESULTS_FILE"
echo "=== Slitherlink Comprehensive Benchmark Suite ===" > "$DETAILED_LOG"
echo "Date: $(date)" >> "$DETAILED_LOG"
echo "" >> "$DETAILED_LOG"

# Test definitions: file:size:difficulty:timeout
declare -a TESTS=(
    # 4×4 puzzles - EASY
    "puzzles/examples/example4x4.txt:4x4:Easy:10"
    
    # 5×5 puzzles - EASY to MEDIUM
    "puzzles/examples/example5x5.txt:5x5:Medium:30"
    
    # 6×6 puzzles - MEDIUM
    "puzzles/examples/example6x6.txt:6x6:Medium:60"
    
    # 7×7 puzzles - MEDIUM
    "puzzles/examples/example7x7.txt:7x7:Medium:120"
    
    # 8×8 puzzles - MEDIUM to HARD
    "puzzles/examples/example8x8_simple.txt:8x8:Easy:30"
    "puzzles/examples/example8x8.txt:8x8:Hard:180"
    "puzzles/examples/example8x8_box.txt:8x8:Medium:90"
    
    # 10×10 puzzles - HARD to VERY HARD
    "puzzles/examples/example10x10_dense.txt:10x10:Hard:180"
    "puzzles/examples/example10x10.txt:10x10:VeryHard:300"
    
    # 12×12 puzzles - EXTREME
    "puzzles/examples/example12x12_simple.txt:12x12:Hard:600"
    "puzzles/examples/example12x12.txt:12x12:Extreme:1200"
    
    # 15×15 puzzles - EXTREME+
    "puzzles/examples/example15x15.txt:15x15:Extreme:1800"
    
    # 20×20 puzzles - NIGHTMARE
    "puzzles/examples/example20x20_dense.txt:20x20:Nightmare:3600"
    "puzzles/examples/example20x20.txt:20x20:Nightmare:3600"
    "example8x8.txt:8x8:Hard:180"
    "example8x8_box.txt:8x8:Medium:90"
    
    # 10×10 puzzles - HARD to VERY HARD
    "example10x10_dense.txt:10x10:Hard:180"
    "example10x10.txt:10x10:VeryHard:300"
    
    # 12×12 puzzles - EXTREME
    "example12x12_simple.txt:12x12:Hard:600"
    "example12x12.txt:12x12:Extreme:1200"
    
    # 15×15 puzzles - EXTREME+
    "example15x15.txt:15x15:Extreme:1800"
    
    # 20×20 puzzles - NIGHTMARE
    "example20x20_dense.txt:20x20:Nightmare:3600"
    "example20x20.txt:20x20:Nightmare:3600"
)

# Function to count clues in puzzle file
count_clues() {
    local file=$1
    # Skip first line (dimensions), count non-dot non-space characters
    tail -n +2 "$file" | grep -o '[0-3]' | wc -l | tr -d ' '
}

# Function to calculate density
calculate_density() {
    local file=$1
    # Read dimensions from first line
    read -r rows cols < "$file"
    local total_cells=$((rows * cols))
    local clues=$(count_clues "$file")
    
    if [ $total_cells -gt 0 ]; then
        echo "scale=3; $clues / $total_cells" | bc
    else
        echo "0"
    fi
}

# Function to run a single test
run_test() {
    local file=$1
    local size=$2
    local difficulty=$3
    local timeout=$4
    
    if [ ! -f "$file" ]; then
        echo -e "${YELLOW}⚠ SKIP: $file not found${NC}"
        return
    fi
    
    # Get puzzle statistics
    local clues=$(count_clues "$file")
    local density=$(calculate_density "$file")
    
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}Testing:${NC} $file"
    echo -e "${BLUE}Size:${NC} $size | ${BLUE}Difficulty:${NC} $difficulty"
    echo -e "${BLUE}Clues:${NC} $clues | ${BLUE}Density:${NC} ${density}"
    echo -e "${BLUE}Timeout:${NC} ${timeout}s"
    echo ""
    
    # Run with timeout and capture timing
    local start=$(date +%s.%N)
    local output
    local exit_code
    
    if command -v gtimeout &> /dev/null; then
        # macOS with GNU timeout (installed via brew install coreutils)
        output=$(gtimeout $timeout "$SOLVER" "$file" 2>&1) || exit_code=$?
    elif command -v timeout &> /dev/null; then
        # Linux timeout
        output=$(timeout $timeout "$SOLVER" "$file" 2>&1) || exit_code=$?
    else
        # No timeout available - run without
        echo -e "${YELLOW}Warning: timeout command not found, running without timeout${NC}"
        output=$("$SOLVER" "$file" 2>&1) || exit_code=$?
    fi
    
    local end=$(date +%s.%N)
    local runtime=$(echo "$end - $start" | bc)
    
    # Parse output
    local solutions=$(echo "$output" | grep -o "Found [0-9]* solution" | grep -o "[0-9]*" || echo "0")
    local status="UNKNOWN"
    
    if [ "$exit_code" == "124" ]; then
        status="TIMEOUT"
        echo -e "${RED}⏱ TIMEOUT after ${timeout}s${NC}"
    elif echo "$output" | grep -q "Found [0-9]* solution"; then
        if [ "$solutions" -gt 0 ]; then
            status="SUCCESS"
            echo -e "${GREEN}✓ SUCCESS - Found $solutions solution(s)${NC}"
            echo -e "${GREEN}⏱ Time: ${runtime}s${NC}"
        else
            status="NO_SOLUTION"
            echo -e "${YELLOW}⚠ No solutions found${NC}"
        fi
    else
        status="ERROR"
        echo -e "${RED}✗ ERROR${NC}"
    fi
    
    # Save to CSV
    echo "$file,$size,$difficulty,$clues,$density,$runtime,$solutions,$status" >> "$RESULTS_FILE"
    
    # Save detailed output to log
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" >> "$DETAILED_LOG"
    echo "Test: $file | Size: $size | Difficulty: $difficulty" >> "$DETAILED_LOG"
    echo "Clues: $clues | Density: $density | Time: ${runtime}s | Status: $status" >> "$DETAILED_LOG"
    echo "Output:" >> "$DETAILED_LOG"
    echo "$output" >> "$DETAILED_LOG"
    echo "" >> "$DETAILED_LOG"
    
    echo ""
}

# Main execution
echo -e "${CYAN}╔════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║  Slitherlink Comprehensive Benchmark Suite    ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════════╝${NC}"
echo ""

# Run all tests
for test_def in "${TESTS[@]}"; do
    IFS=':' read -r file size difficulty timeout <<< "$test_def"
    run_test "$file" "$size" "$difficulty" "$timeout"
done

# Summary
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}╔════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║            Benchmark Complete!                 ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${BLUE}Results saved to:${NC}"
echo -e "  • $RESULTS_FILE (CSV format)"
echo -e "  • $DETAILED_LOG (detailed log)"
echo ""

# Count results by status
if [ -f "$RESULTS_FILE" ]; then
    local total=$(tail -n +2 "$RESULTS_FILE" | wc -l | tr -d ' ')
    local success=$(grep -c ",SUCCESS$" "$RESULTS_FILE" || echo "0")
    local timeout=$(grep -c ",TIMEOUT$" "$RESULTS_FILE" || echo "0")
    local error=$(grep -c ",ERROR$" "$RESULTS_FILE" || echo "0")
    
    echo -e "${BLUE}Summary:${NC}"
    echo -e "  Total tests: $total"
    echo -e "  ${GREEN}Success: $success${NC}"
    echo -e "  ${RED}Timeout: $timeout${NC}"
    echo -e "  ${RED}Errors: $error${NC}"
    echo ""
fi

# Generate quick summary table
echo -e "${BLUE}Quick Results:${NC}"
column -t -s',' "$RESULTS_FILE" | head -20

echo ""
echo -e "${CYAN}Use 'cat $RESULTS_FILE' to see full CSV results${NC}"
echo -e "${CYAN}Use 'cat $DETAILED_LOG' to see detailed output${NC}"
