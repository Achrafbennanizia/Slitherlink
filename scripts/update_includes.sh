#!/bin/bash
# Script to update all include paths in source files

cd /Users/achraf/CLionProjects/Slitherlink

echo "Updating include paths in source files..."

# Update all .cpp files to use new include paths
find src -name "*.cpp" -type f -exec sed -i.bak \
    -e 's|#include "Grid.h"|#include "slitherlink/core/Grid.h"|g' \
    -e 's|#include "Edge.h"|#include "slitherlink/core/Edge.h"|g' \
    -e 's|#include "State.h"|#include "slitherlink/core/State.h"|g' \
    -e 's|#include "Solution.h"|#include "slitherlink/core/Solution.h"|g' \
    -e 's|#include "Solver.h"|#include "slitherlink/solver/Solver.h"|g' \
    -e 's|#include "GridReader.h"|#include "slitherlink/io/GridReader.h"|g' \
    -e 's|#include "SolutionPrinter.h"|#include "slitherlink/io/SolutionPrinter.h"|g' \
    -e 's|#include "SolutionCollector.h"|#include "slitherlink/io/SolutionCollector.h"|g' \
    -e 's|#include "GraphBuilder.h"|#include "slitherlink/solver/GraphBuilder.h"|g' \
    -e 's|#include "SlitherlinkSolver.h"|#include "slitherlink/factory/SlitherlinkSolver.h"|g' \
    -e 's|#include "SolverFactory.h"|#include "slitherlink/factory/SolverFactory.h"|g' \
    -e 's|#include "IGraphBuilder.h"|#include "slitherlink/interfaces/IGraphBuilder.h"|g' \
    -e 's|#include "IHeuristic.h"|#include "slitherlink/interfaces/IHeuristic.h"|g' \
    -e 's|#include "ISolutionCollector.h"|#include "slitherlink/interfaces/ISolutionCollector.h"|g' \
    -e 's|#include "ISolutionPrinter.h"|#include "slitherlink/interfaces/ISolutionPrinter.h"|g' \
    {} \;

# Update all .h files to use new include paths
find include/slitherlink -name "*.h" -type f -exec sed -i.bak \
    -e 's|#include "Grid.h"|#include "slitherlink/core/Grid.h"|g' \
    -e 's|#include "Edge.h"|#include "slitherlink/core/Edge.h"|g' \
    -e 's|#include "State.h"|#include "slitherlink/core/State.h"|g' \
    -e 's|#include "Solution.h"|#include "slitherlink/core/Solution.h"|g' \
    -e 's|#include "Solver.h"|#include "slitherlink/solver/Solver.h"|g' \
    -e 's|#include "GridReader.h"|#include "slitherlink/io/GridReader.h"|g' \
    -e 's|#include "SolutionPrinter.h"|#include "slitherlink/io/SolutionPrinter.h"|g' \
    -e 's|#include "SolutionCollector.h"|#include "slitherlink/io/SolutionCollector.h"|g' \
    -e 's|#include "GraphBuilder.h"|#include "slitherlink/solver/GraphBuilder.h"|g' \
    -e 's|#include "SlitherlinkSolver.h"|#include "slitherlink/factory/SlitherlinkSolver.h"|g' \
    -e 's|#include "SolverFactory.h"|#include "slitherlink/factory/SolverFactory.h"|g' \
    -e 's|#include "IGraphBuilder.h"|#include "slitherlink/interfaces/IGraphBuilder.h"|g' \
    -e 's|#include "IHeuristic.h"|#include "slitherlink/interfaces/IHeuristic.h"|g' \
    -e 's|#include "ISolutionCollector.h"|#include "slitherlink/interfaces/ISolutionCollector.h"|g' \
    -e 's|#include "ISolutionPrinter.h"|#include "slitherlink/interfaces/ISolutionPrinter.h"|g' \
    -e 's|#include "IConstraintPropagator.h"|#include "slitherlink/interfaces/IConstraintPropagator.h"|g' \
    {} \;

# Clean up backup files
find . -name "*.bak" -delete

echo "Include paths updated successfully!"
