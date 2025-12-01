#!/bin/bash

# Quick Sample Benchmark - Tests representative puzzles from each size
SOLVER="./cmake-build-debug/slitherlink"
PUZZLE_DIR="puzzles/examples"

echo "=== QUICK SAMPLE BENCHMARK ==="
echo "Testing representative puzzles from each size"
echo ""

# Sample puzzles - one from each size category
declare -a SAMPLES=(
    "4x4:example4x4_medium.txt:Medium"
    "5x5:example5x5_medium.txt:Medium"
    "6x6:example6x6_medium.txt:Medium"
    "7x7:example7x7_medium.txt:Medium"
    "8x8:example8x8_medium.txt:Medium"
    "8x8:example8x8_simple.txt:Easy"
    "10x10:example10x10_medium.txt:Medium"
    "10x10:example10x10_dense.txt:Easy"
    "12x12:example12x12_medium.txt:Medium"
    "15x15:example15x15_medium.txt:Medium"
    "20x20:example20x20_medium.txt:Medium"
)

printf "%-10s %-30s %-10s %-15s %s\n" "SIZE" "FILE" "DIFFICULTY" "TIME" "RESULT"
echo "────────────────────────────────────────────────────────────────────────"

for sample in "${SAMPLES[@]}"; do
    IFS=':' read -r size file diff <<< "$sample"
    filepath="$PUZZLE_DIR/$file"
    
    if [ ! -f "$filepath" ]; then
        printf "%-10s %-30s %-10s %-15s %s\n" "$size" "$file" "$diff" "N/A" "FILE NOT FOUND"
        continue
    fi
    
    # Run test with timeout
    start=$(date +%s.%N)
    output=$(gtimeout 30 "$SOLVER" "$filepath" 2>&1)
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
        result="⏱️  TIMEOUT"
    elif echo "$output" | grep -q "Solution.*found"; then
        solutions=$(echo "$output" | grep "Total solutions" | grep -oE '[0-9]+')
        result="✅ SOLVED ($solutions sol)"
    elif echo "$output" | grep -q "No solution"; then
        result="❌ NO SOLUTION"
    else
        result="❌ ERROR"
    fi
    
    printf "%-10s %-30s %-10s %-15s %s\n" "$size" "$file" "$diff" "$time_str" "$result"
done

echo ""
echo "Sample test complete!"
