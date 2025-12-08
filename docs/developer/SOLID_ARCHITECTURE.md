# SOLID Architecture Refactoring - Design Document

> Design reference for the SOLID-oriented variant. The active layout/entrypoints are covered in `README.md` and `docs/developer/ARCHITECTURE.md`.

**Date**: December 6, 2025  
**Status**: 🚧 In Progress - Foundation Complete  
**Goal**: Refactor monolithic `main.cpp` into SOLID-compliant modular architecture

---

## SOLID Principles Applied

### 1. Single Responsibility Principle (SRP)

Each class has one reason to change:

- **Grid**: Manages puzzle grid data only
- **State**: Manages search state data only
- **Solution**: Represents solution data only
- **Edge**: Represents edge structure only
- **SmartHeuristic**: Edge selection logic only
- **Validator**: State validation logic only
- **Propagator**: Constraint propagation only

### 2. Open/Closed Principle (OCP)

Open for extension, closed for modification:

- **IHeuristic**: Can add new heuristics without modifying existing code
- **IValidator**: Can add new validation strategies
- **IPropagator**: Can add new propagation algorithms

### 3. Liskov Substitution Principle (LSP)

Interfaces can be substituted:

- Any `IHeuristic` implementation can replace another
- Any `IPropagator` can be swapped
- Polymorphism enables strategy pattern

### 4. Interface Segregation Principle (ISP)

Clients don't depend on unused interfaces:

- **IHeuristic**: Only edge selection
- **IValidator**: Only validation methods
- **IPropagator**: Only propagation methods
- No monolithic interface forcing unused methods

### 5. Dependency Inversion Principle (DIP)

Depend on abstractions, not concretions:

- Solver depends on `IHeuristic`, not `SmartHeuristic`
- Solver depends on `IPropagator`, not concrete propagator
- Dependency injection enables testing and flexibility

---

## Architecture Overview

```
slitherlink/
├── include/           # Header files (interfaces + classes)
│   ├── Grid.h         # Grid data structure
│   ├── Edge.h         # Edge structure
│   ├── State.h        # Search state
│   ├── Solution.h     # Solution representation
│   ├── IHeuristic.h   # Heuristic interface
│   ├── IValidator.h   # Validator interface
│   ├── IPropagator.h  # Propagator interface
│   └── SmartHeuristic.h  # Smart heuristic implementation
├── src/               # Implementation files
│   ├── Grid.cpp
│   ├── State.cpp
│   └── SmartHeuristic.cpp
└── main.cpp           # Entry point (to be refactored)
```

---

## Component Responsibilities

### Core Data Structures

#### Grid

```cpp
class Grid {
    // Single Responsibility: Grid data management
    int getRows(), getCols();
    int getClue(row, col);
    bool loadFromFile(filename);
};
```

#### State

```cpp
class State {
    // Single Responsibility: Search state data
    char getEdgeState(idx);
    int getPointDegree(idx);
    void initialize(edgeCount, pointCount, cellCount);
};
```

#### Solution

```cpp
class Solution {
    // Single Responsibility: Solution data
    const vector<char>& getEdgeState();
    const vector<pair<int,int>>& getCyclePoints();
};
```

### Strategy Interfaces

#### IHeuristic (Strategy Pattern)

```cpp
class IHeuristic {
    virtual int selectNextEdge(const State& state) = 0;
};

// Implementations:
// - SmartHeuristic (Phase 2 optimization)
// - SimpleHeuristic (basic scoring)
// - RandomHeuristic (for testing)
```

#### IValidator (Strategy Pattern)

```cpp
class IValidator {
    virtual bool isValid(const State& state) = 0;
    virtual bool isUnsolvable(const State& state) = 0;
};

// Implementations:
// - StandardValidator (current logic)
// - ParallelValidator (TBB version)
```

#### IPropagator (Strategy Pattern)

```cpp
class IPropagator {
    virtual bool propagate(State& state) = 0;
    virtual bool applyDecision(State& state, int edge, int val) = 0;
};

// Implementations:
// - OptimizedPropagator (Phase 2 queue optimization)
// - SimplePropagator (basic algorithm)
```

---

## Planned Refactoring Steps

### ✅ Phase 1: Foundation (COMPLETED)

- [x] Create directory structure (`include/`, `src/`)
- [x] Define core data classes (Grid, State, Solution, Edge)
- [x] Define strategy interfaces (IHeuristic, IValidator, IPropagator)
- [x] Implement SmartHeuristic (Phase 2 optimization #11)

### 🚧 Phase 2: Core Implementations (IN PROGRESS)

- [ ] Implement Validator classes
- [ ] Implement Propagator classes
- [ ] Create EdgeGraph builder
- [ ] Create SolutionChecker

### ⏳ Phase 3: Solver Refactoring (TODO)

- [ ] Create Solver class with dependency injection
- [ ] Constructor: `Solver(Grid, IHeuristic*, IValidator*, IPropagator*)`
- [ ] Refactor search function
- [ ] Add configuration class for solver options

### ⏳ Phase 4: Testing & Integration (TODO)

- [ ] Update CMakeLists.txt for new structure
- [ ] Ensure all optimizations preserved
- [ ] Run performance benchmarks
- [ ] Verify correctness with unit tests

---

## Example Usage (After Refactoring)

```cpp
#include "Solver.h"
#include "SmartHeuristic.h"
#include "OptimizedPropagator.h"
#include "StandardValidator.h"

int main() {
    // Load puzzle
    Grid grid;
    grid.loadFromFile("puzzle.txt");

    // Build edge graph
    EdgeGraphBuilder builder(grid);
    auto edges = builder.build();

    // Create strategies (Dependency Injection)
    auto heuristic = std::make_unique<SmartHeuristic>(grid, edges);
    auto propagator = std::make_unique<OptimizedPropagator>(grid, edges);
    auto validator = std::make_unique<StandardValidator>(grid, edges);

    // Configure solver
    SolverConfig config;
    config.threads = 8;
    config.findAll = false;

    // Create solver (Dependency Inversion)
    Solver solver(grid, edges,
                  std::move(heuristic),
                  std::move(propagator),
                  std::move(validator),
                  config);

    // Solve
    auto solutions = solver.solve();

    // Output
    for (const auto& sol : solutions) {
        printSolution(sol);
    }
}
```

---

## Benefits of SOLID Architecture

### Maintainability

- Each class has clear, focused responsibility
- Easy to locate and fix bugs
- Changes localized to specific components

### Testability

- Mock interfaces for unit testing
- Test heuristics independently
- Test validators independently
- Integration tests with real components

### Extensibility

- Add new heuristics without touching solver
- Swap propagation algorithms
- Try different validation strategies
- A/B test optimization approaches

### Performance

- All Phase 1-3 optimizations preserved
- Can benchmark different strategies
- Easy to enable/disable optimizations
- Clear separation of hot paths

### Code Reuse

- Heuristics can be reused in other solvers
- Propagators can be shared across projects
- Grid and State classes are general-purpose

---

## Performance Preservation

The refactoring maintains all optimizations:

- **Phase 1**: Compilation flags, vector reservation, move semantics
- **Phase 2**: Smart heuristics, parallel pruning, optimized queues, inlining
- **Phase 3**: Memory pool, cache-friendly layout, symmetry breaking

Performance testing after refactoring should show:

- Same or better performance (inlining, devirtualization)
- Easier to add new optimizations
- Clearer performance bottleneck identification

---

## Migration Strategy

### Option 1: Gradual Migration (Recommended)

1. Keep `main.cpp` working
2. Add new classes alongside
3. Gradually move code to new structure
4. Switch over when complete
5. Remove old code

### Option 2: Clean Break

1. Create new `main_v2.cpp`
2. Build entire new architecture
3. Test thoroughly
4. Replace old main
5. Risk: longer before working version

---

## Current Status

### Completed (✅)

- Directory structure
- Core data classes (Grid, State, Solution, Edge)
- Strategy interfaces (IHeuristic, IValidator, IPropagator)
- SmartHeuristic implementation
- Grid loading logic
- State initialization

### In Progress (🚧)

- Validator implementations
- Propagator implementations
- Solver class design

### Not Started (⏳)

- EdgeGraphBuilder
- SolutionChecker
- Complete Solver refactoring
- CMakeLists.txt updates
- Integration testing

---

## Next Steps

1. **Implement Validator classes** with all Phase 2 optimizations
2. **Implement Propagator classes** with optimized queue logic
3. **Create Solver class** with dependency injection
4. **Update build system** to compile new structure
5. **Run benchmarks** to ensure no performance regression
6. **Migrate main.cpp** to use new architecture

---

## Estimated Effort

- **Foundation**: ✅ 1 hour (DONE)
- **Core implementations**: 2-3 hours
- **Solver refactoring**: 3-4 hours
- **Testing & validation**: 2-3 hours
- **Documentation**: 1 hour

**Total**: ~10-12 hours for complete SOLID refactoring

---

## Notes

This refactoring prioritizes:

1. **Correctness**: Must preserve all functionality
2. **Performance**: Must maintain 570x speedup
3. **Clean code**: SOLID principles throughout
4. **Testability**: Easy to unit test components
5. **Extensibility**: Easy to add new strategies

The architecture enables future enhancements like:

- GUI integration
- Alternative search algorithms
- Machine learning heuristics
- Distributed solving
- Puzzle generation tools

---

## Conclusion

The SOLID refactoring provides a **clean, maintainable, testable** architecture while **preserving all performance optimizations**. The strategy pattern enables easy experimentation with different algorithms, and dependency injection makes testing straightforward.

**Status**: Foundation complete, ready to continue implementation.
