# Slitherlink Solver - Improved Architecture

> This document captures an aspirational SOLID/modular layout. The current working layout is described in `README.md` and `docs/developer/ARCHITECTURE.md`; use this file as a design reference, not the live tree.

## Overview

This document describes the improved, modular architecture of the Slitherlink solver, designed following modern C++ best practices and SOLID principles.

## Directory Structure

```
Slitherlink/
├── include/slitherlink/          # Public API headers
│   ├── Slitherlink.h             # Main public API (include this one file)
│   ├── Common.h                  # Common types and forward declarations
│   ├── Config.h                  # Configuration structures
│   │
│   ├── core/                     # Core data structures
│   │   ├── Grid.h                # Puzzle grid representation
│   │   ├── Edge.h                # Graph edge structure
│   │   ├── State.h               # Search state representation
│   │   └── Solution.h            # Solution structure
│   │
│   ├── interfaces/               # SOLID interfaces (ISP)
│   │   ├── IGraphBuilder.h       # Graph construction interface
│   │   ├── IConstraintPropagator.h
│   │   ├── IHeuristic.h          # Edge selection heuristic
│   │   ├── ISolutionCollector.h  # Solution collection strategy
│   │   ├── ISolutionPrinter.h    # Output formatting
│   │   └── ISolutionValidator.h  # Solution validation
│   │
│   ├── solver/                   # Solver components
│   │   ├── Solver.h              # Main backtracking solver
│   │   ├── GraphBuilder.h        # Concrete graph builder
│   │   ├── ConstraintPropagator.h
│   │   └── EdgeSelectionHeuristic.h
│   │
│   ├── io/                       # Input/Output components
│   │   ├── GridReader.h          # Puzzle file reader
│   │   ├── SolutionPrinter.h     # Solution output formatter
│   │   └── SolutionCollector.h   # Solution collection
│   │
│   ├── factory/                  # Factory pattern (OCP/DIP)
│   │   ├── SolverFactory.h       # Creates configured solvers
│   │   └── SlitherlinkSolver.h   # Facade for easy usage
│   │
│   └── utils/                    # Utility classes
│       ├── Logger.h              # Thread-safe logging
│       └── Timer.h               # Performance measurement
│
├── src/                          # Implementation files
│   ├── main.cpp                  # CLI application entry point
│   │
│   ├── core/                     # Core implementations
│   │   └── Config.cpp
│   │
│   ├── solver/                   # Solver implementations
│   │   ├── Solver.cpp
│   │   └── GraphBuilder.cpp
│   │
│   ├── io/                       # I/O implementations
│   │   ├── GridReader.cpp
│   │   ├── SolutionPrinter.cpp
│   │   └── SolutionCollector.cpp
│   │
│   └── factory/                  # Factory implementations
│       ├── SolverFactory.cpp
│       └── SlitherlinkSolver.cpp
│
├── tests/                        # Unit tests (optional)
├── puzzles/                      # Example puzzle files
├── docs/                         # Documentation
└── CMakeLists.txt                # Modern CMake build configuration
```

## Architecture Layers

### 1. **Public API Layer** (`include/slitherlink/`)

- **Purpose**: Clean, stable interface for library users
- **Key Files**:
  - `Slitherlink.h` - Single-header include for easy integration
  - `Config.h` - Configuration structures
  - `Common.h` - Common types and constants

**Usage Example**:

```cpp
#include <slitherlink/Slitherlink.h>

// Simple one-liner
auto solutions = slitherlink::SlitherlinkAPI::solve("puzzle.txt");

// Or with configuration
slitherlink::SolverConfig config;
config.maxSolutions = 5;
config.numThreads = 8;
auto solutions = slitherlink::SlitherlinkAPI::solve("puzzle.txt", config);
```

### 2. **Core Data Layer** (`core/`)

- **Purpose**: Fundamental data structures
- **Components**:
  - `Grid` - Puzzle representation
  - `Edge` - Graph edge
  - `State` - Search state
  - `Solution` - Solution representation

**Design**: Plain structs for performance, move semantics enabled

### 3. **Interface Layer** (`interfaces/`)

- **Purpose**: SOLID Interface Segregation Principle
- **Benefits**:
  - Easy mocking for tests
  - Multiple implementations possible
  - Loose coupling
  - Open/Closed Principle support

**Interfaces**:

- `IGraphBuilder` - Converts Grid → Graph
- `IConstraintPropagator` - Applies constraint propagation
- `IHeuristic` - Edge selection strategy
- `ISolutionCollector` - Solution storage strategy
- `ISolutionPrinter` - Output formatting strategy
- `ISolutionValidator` - Solution validation

### 4. **Solver Layer** (`solver/`)

- **Purpose**: Core solving algorithms
- **Components**:
  - `Solver` - Backtracking search algorithm
  - `GraphBuilder` - Graph construction
  - `ConstraintPropagator` - AC-3 style propagation
  - `EdgeSelectionHeuristic` - MRV/degree heuristic

**Design**: Implements interfaces, dependency injection

### 5. **I/O Layer** (`io/`)

- **Purpose**: Input/Output operations
- **Components**:
  - `GridReader` - Parses puzzle files
  - `SolutionPrinter` - Formats solutions
  - `SolutionCollector` - Collects and stores solutions

### 6. **Factory Layer** (`factory/`)

- **Purpose**: Object creation with dependency injection
- **Components**:
  - `SolverFactory` - Creates configured solvers
  - `SlitherlinkSolver` - Facade pattern for simplified usage

**Design**: Factory Method + Dependency Inversion Principle

### 7. **Utilities Layer** (`utils/`)

- **Purpose**: Cross-cutting concerns
- **Components**:
  - `Logger` - Thread-safe logging
  - `Timer` - Performance measurement

## Build System Architecture

### CMake Targets

```cmake
# 1. Library Target (Reusable)
slitherlink_lib (STATIC/SHARED)
  ├── Public Interface Headers
  ├── Private Implementation
  └── Dependencies: TBB, Threads

# 2. Executable Target (CLI)
slitherlink (EXECUTABLE)
  └── Links: slitherlink_lib

# 3. Test Target (Optional)
slitherlink_tests
  └── Links: slitherlink_lib, GoogleTest
```

### Build Options

```cmake
SLITHERLINK_BUILD_SHARED_LIBS   # Build shared library
SLITHERLINK_BUILD_TESTS          # Build unit tests
SLITHERLINK_BUILD_EXAMPLES       # Build examples
SLITHERLINK_WARNINGS_AS_ERRORS   # Strict compilation
SLITHERLINK_ENABLE_SANITIZERS    # Memory/UB sanitizers
```

## Namespace Organization

All code is wrapped in the `slitherlink` namespace:

```cpp
namespace slitherlink {
    // Core types
    struct Grid { ... };
    struct Solution { ... };

    // Nested namespaces for organization
    namespace utils {
        class Logger { ... };
        class Timer { ... };
    }
}
```

## Dependency Management

### External Dependencies

1. **Intel TBB** - Parallel task scheduling

   - Optional but recommended
   - Detected via CMake find_package

2. **C++ Standard Library** - STL containers, algorithms

   - Required: C++17 minimum

3. **Threads** - std::thread, std::mutex
   - System threading library

### Internal Dependencies

```
main.cpp
  └── SolverFactory
        └── SlitherlinkSolver (Facade)
              ├── Solver
              │     ├── GraphBuilder → IGraphBuilder
              │     ├── ConstraintPropagator → IConstraintPropagator
              │     └── EdgeSelectionHeuristic → IHeuristic
              ├── SolutionCollector → ISolutionCollector
              └── SolutionPrinter → ISolutionPrinter
```

## SOLID Principles Implementation

### Single Responsibility Principle (SRP)

- Each class has one reason to change
- Example: `GridReader` only reads files, `Solver` only solves

### Open/Closed Principle (OCP)

- Extend via new implementations, not modifications
- Example: Add new heuristic by implementing `IHeuristic`

### Liskov Substitution Principle (LSP)

- Interfaces are properly abstracted
- Any `IHeuristic` can replace another

### Interface Segregation Principle (ISP)

- 6 small, focused interfaces
- Clients depend only on methods they use

### Dependency Inversion Principle (DIP)

- High-level code depends on interfaces
- `Solver` depends on `IGraphBuilder`, not concrete `GraphBuilder`

## Performance Considerations

### Memory Efficiency

- Move semantics throughout
- Memory pooling support (optional)
- Bit-packed states (optional)

### Parallelization

- TBB task parallelism
- Lock-free solution collection
- Thread-safe logging

### Modularity Benefits

- Incremental compilation
- Easy testing (mock interfaces)
- Library reuse in other projects

## Extension Points

### Adding New Heuristic

```cpp
class MyHeuristic : public IHeuristic {
    int selectNextEdge(const State &s) const override {
        // Your custom logic
    }
};
```

### Adding New Output Format

```cpp
class JSONPrinter : public ISolutionPrinter {
    void printSolution(const Solution &sol) override {
        // JSON formatting
    }
};
```

### Custom Configuration

```cpp
SolverConfig config;
config.numThreads = 16;
config.enableEarlyPruning = true;
config.verbose = true;

auto solver = SolverFactory::createSolver(config);
```

## Migration from Old Architecture

### Old (Monolithic)

```
main.cpp (1000+ lines)
  └── Everything in one file
```

### New (Modular)

```
17 header files + 8 source files
  └── Organized by responsibility
  └── Reusable library
  └── SOLID compliant
```

## Benefits of New Architecture

✅ **Maintainability**: Clear separation of concerns
✅ **Testability**: Interfaces enable mocking
✅ **Reusability**: Library can be used in other projects
✅ **Extensibility**: Easy to add new features
✅ **Performance**: No overhead from good design
✅ **Documentation**: Self-documenting structure
✅ **Build Speed**: Incremental compilation
✅ **Professional**: Industry-standard practices

## Version

Current: **v0.2.0**

- Major architectural refactoring
- Library + executable separation
- SOLID principles throughout
- Modern CMake configuration
