#!/bin/bash
echo "=== Slitherlink Solver Test Suite ==="
echo ""

tests=(
    "example4x4.txt:10"
    "example5x5.txt:30"
    "example6x6.txt:160"
    "example7x7.txt:160"
    "example8x8.txt:180"
)

for test in "${tests[@]}"; do
    IFS=':' read -r file timeout <<< "$test"
    echo "Testing $file (timeout: ${timeout}s)..."
    
    start=$(date +%s.%N)
    if ./cmake-build-debug/slitherlink "$file" 2>&1 | grep -q "Solution 1 found"; then
        end=$(date +%s.%N)
        runtime=$(echo "$end - $start" | bc)
        echo "✅ PASSED - Time: ${runtime}s"
    else
        echo "❌ FAILED or NO SOLUTION"
    fi
    echo ""
done

echo "=== Test Summary Complete ==="
