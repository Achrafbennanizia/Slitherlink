#!/bin/bash
# Compile all historical versions of Slitherlink solver
# Usage: ./compile_all_versions.sh

echo "🔨 Compiling Slitherlink Historical Versions"
echo "============================================"

# Detect TBB installation
if [ -d "/opt/homebrew/include" ]; then
    TBB_INCLUDE="/opt/homebrew/include"
    TBB_LIB="/opt/homebrew/lib"
    echo "📦 Using Homebrew TBB (macOS)"
elif [ -d "/opt/intel/oneapi/tbb/latest/include" ]; then
    TBB_INCLUDE="/opt/intel/oneapi/tbb/latest/include"
    TBB_LIB="/opt/intel/oneapi/tbb/latest/lib"
    echo "📦 Using Intel oneAPI TBB"
else
    echo "❌ TBB not found. Please install Intel TBB:"
    echo "   macOS: brew install tbb"
    echo "   Linux: Install Intel oneAPI toolkit"
    exit 1
fi

# Compilation flags
CXX="g++"
CXXFLAGS="-std=c++17 -O3"
TBB_FLAGS="-DUSE_TBB -I$TBB_INCLUDE -L$TBB_LIB -ltbb"

cd versions

echo ""
echo "Version 1: Baseline (no TBB)"
$CXX $CXXFLAGS v01_baseline.cpp -o ../slitherlink_v01 && echo "✅ v01_baseline compiled"

echo ""
echo "Version 2-10: With TBB"
for v in 02 03 04 05 06 08 09 10; do
    if [ -f "v${v}_*.cpp" ]; then
        file=$(ls v${v}_*.cpp 2>/dev/null | head -1)
        if [ -f "$file" ]; then
            $CXX $CXXFLAGS $TBB_FLAGS "$file" -o ../slitherlink_v${v} && \
                echo "✅ v${v} compiled ($file)"
        fi
    fi
done

# Note about V7
if [ -f "v07_ortools_failed.cpp" ]; then
    echo ""
    echo "⚠️  Version 7 (OR-Tools) available but not compiled"
    echo "    (Requires OR-Tools library - failed experiment)"
fi

cd ..

echo ""
echo "🎉 Compilation Complete!"
echo ""
echo "Available executables:"
ls -lh slitherlink_v* 2>/dev/null | awk '{print $9, $5}'

echo ""
echo "📊 To benchmark all versions:"
echo "   ./benchmark_versions.sh example8x8.txt"
