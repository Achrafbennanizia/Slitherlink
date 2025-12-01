#!/bin/bash

# Test all existing original puzzles
SOLVER="./cmake-build-debug/slitherlink"
PUZZLE_DIR="puzzles/examples"

echo "=== TESTING EXISTING PUZZLES ==="
echo ""

# List of original puzzles that were working before
ORIGINALS=(
    "example4x4.txt"
    "example5x5.txt"
    "example6x6.txt"
    "example7x7.txt"
    "example8x8.txt"
    "example8x8_simple.txt"
    "example8x8_box.txt"
    "example10x10.txt"
    "example10x10_dense.txt"
    "example12x12.txt"
    "example12x12_simple.txt"
    "example15x15.txt"
    "example20x20.txt"
    "example20x20_dense.txt"
)

printf "%-30s %-15s %s\n" "FILE" "TIME" "RESULT"
echo "────────────────────────────────────────────────────────────"

for file in "${ORIGINALS[@]}"; do
    filepath="$PUZZLE_DIR/$file"
    
    if [ ! -f "$filepath" ]; then
        printf "%-30s %-15s %s\n" "$file" "N/A" "NOT FOUND"
        continue
    fi
    
    # Run test with timeout
    start=$(date +%s.%N)
    output=$(gtimeout 60 "$SOLVER" "$filepath" 2>&1)
    exit_code=$?
    end=$(date +%s.%N)
    duration=$(echo "$end - $start" | bc)
    
    # Format time
    if (( $(echo "$duration < 1" | bc -l) )); then
        time_str=$(printf "%.0fms" $(echo "$duration * 1000" | bc))
    else
        time_str=$(printf "%.2fs" "$duration")
    fi
    
    # Check result
    if [ $exit_code -eq 124 ]; then
        result="⏱️  TIMEOUT (>60s)"
    elif echo "$output" | grep -q "Solution.*found"; then
        solutions=$(echo "$output" | grep "Total solutions" | grep -oE '[0-9]+')
        nodes=$(echo "$output" | grep -i "nodes" | grep -oE '[0-9,]+' | head -1 | tr -d ',')
        result="✅ SOLVED ($solutions sol, $nodes nodes)"
    elif echo "$output" | grep -q "No solution"; then
        result="❌ NO SOLUTION"
    else
        result="❌ ERROR"
    fi
    
    printf "%-30s %-15s %s\n" "$file" "$time_str" "$result"
done

echo ""
echo "Original puzzles test complete!"
