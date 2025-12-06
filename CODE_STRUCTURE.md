# Code Structure

## Overview

The Slitherlink solver has been reorganized into a clean, modular structure with proper separation of concerns.

## Directory Structure

```
Slitherlink/
├── include/              # Header files
│   ├── Edge.h           # Edge structure definition
│   ├── Grid.h           # Grid structure definition
│   ├── GridReader.h     # Grid file parser interface
│   ├── Solution.h       # Solution structure definition
│   ├── Solver.h         # Solver class interface
│   └── State.h          # State structure definition
├── src/                  # Source files
│   ├── GridReader.cpp   # Grid file parser implementation
│   ├── Solver.cpp       # Solver class implementation (~830 lines)
│   └── main.cpp         # Application entry point
├── puzzles/              # Test puzzle files
├── scripts/              # Build and deployment scripts
├── CMakeLists.txt        # Build configuration
└── README.md             # Project documentation
```

## Module Descriptions

### Core Data Structures (include/)

#### Grid.h

- Defines the puzzle grid structure
- Contains dimensions (n×m) and clue values
- Provides cell indexing utilities

#### Edge.h

- Represents edges in the puzzle graph
- Stores endpoints (u, v) as point indices
- Tracks adjacent cells (cellA, cellB)

#### State.h

- Represents the current search state
- Tracks edge states (undecided/on/off)
- Maintains point degrees and cell edge counts
- Optimized for efficient state copying

#### Solution.h

- Stores a complete solution
- Contains final edge states
- Ordered cycle of points forming the solution loop

#### Solver.h

- Main solver class interface
- Declares all solving algorithms
- Manages parallelization with TBB

#### GridReader.h

- Interface for parsing puzzle files
- Reads grid dimensions and clue values

### Implementation Files (src/)

#### main.cpp (~50 lines)

- Application entry point
- Command-line argument parsing
- Timing and output coordination

#### Solver.cpp (~830 lines)

- Complete solver implementation
- Methods:
  - `calculateOptimalParallelDepth()` - Dynamic parallelization tuning
  - `buildEdges()` - Graph construction
  - `initialState()` - State initialization
  - `applyDecision()` - Edge decision application
  - `quickValidityCheck()` - Fast constraint validation
  - `propagateConstraints()` - Constraint propagation
  - `selectNextEdge()` - Smart heuristic-based edge selection
  - `finalCheckAndStore()` - Solution verification
  - `search()` - Recursive backtracking with parallelization
  - `run()` - Main solver entry point
  - `printSolution()` - Solution visualization
  - `printSolutions()` - Summary output

#### GridReader.cpp (~60 lines)

- Puzzle file parsing
- Error handling for malformed inputs
- Grid validation

## Build System

### CMakeLists.txt

- Configures multi-file build
- Sets include directories
- Links threading libraries (pthreads, TBB)
- Manages compiler flags and optimization

## Benefits of This Structure

1. **Modularity**: Each component has a single, well-defined responsibility
2. **Maintainability**: Easy to locate and modify specific functionality
3. **Testability**: Individual components can be tested in isolation
4. **Readability**: Smaller files are easier to understand
5. **Scalability**: New features can be added without cluttering existing files
6. **Compilation Speed**: Incremental builds only recompile changed modules
7. **Clean Interfaces**: Header files provide clear API documentation

## Compilation

```bash
cmake -S . -B build
cmake --build build
```

## File Size Summary

- **Headers**: ~50-150 lines each
- **main.cpp**: ~50 lines
- **GridReader.cpp**: ~60 lines
- **Solver.cpp**: ~830 lines (core algorithm)

Total: ~1000 lines distributed across 9 files (previously 1 monolithic file)
