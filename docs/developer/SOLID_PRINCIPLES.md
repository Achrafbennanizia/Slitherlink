# SOLID Principles Implementation

> Design reference for the SOLID-oriented variant. Active layout/entrypoints: see `README.md` and `docs/developer/ARCHITECTURE.md`.

## Overview

The Slitherlink solver has been refactored to follow **SOLID principles**, making it more maintainable, testable, and extensible.

---

## SOLID Principles Applied

### 1. **S**ingle Responsibility Principle (SRP)

**Definition**: A class should have only one reason to change.

#### Implementation:

- **`GraphBuilder`**: Solely responsible for building the puzzle graph structure
- **`SolutionCollector`**: Only handles collecting and storing solutions
- **`SolutionPrinter`**: Exclusively manages solution output formatting
- **`GridReader`**: Handles only file I/O for puzzle loading
- **`SlitherlinkSolver`**: Coordinates the solving process (orchestration only)

**Before (Violation)**:

```cpp
struct Solver {
    // Mixed responsibilities:
    void buildEdges();          // Graph construction
    void search();               // Algorithm logic
    void printSolution();        // Output formatting
    void run();                  // Orchestration
};
```

**After (Compliant)**:

```cpp
class GraphBuilder {
    void buildGraph(const Grid &grid);  // Single responsibility: graph construction
};

class SolutionPrinter {
    void print(const Solution &sol);     // Single responsibility: formatting
};

class SlitherlinkSolver {
    void solve();                        // Single responsibility: orchestration
};
```

---

### 2. **O**pen/Closed Principle (OCP)

**Definition**: Classes should be open for extension but closed for modification.

#### Implementation:

**Interface-Based Design**:

```cpp
// Open for extension via new implementations
class ISolutionCollector {
public:
    virtual void addSolution(const Solution &solution) = 0;
    virtual bool shouldStop() const = 0;
};

// Concrete implementation
class SolutionCollector : public ISolutionCollector {
    // Can be extended without modifying the interface
};

// Future extensions possible:
class ParallelSolutionCollector : public ISolutionCollector { /*...*/ };
class DatabaseSolutionCollector : public ISolutionCollector { /*...*/ };
```

**Factory Pattern**:

```cpp
class SolverFactory {
public:
    static std::unique_ptr<SlitherlinkSolver> createSolver(
        const Grid &grid, bool findAll
    );
    // New solver types can be added without modifying existing code
};
```

---

### 3. **L**iskov Substitution Principle (LSP)

**Definition**: Objects should be replaceable with instances of their subtypes without altering correctness.

#### Implementation:

All interfaces can be substituted with any implementation:

```cpp
// Any ISolutionPrinter implementation can replace another
std::shared_ptr<ISolutionPrinter> printer;

printer = std::make_shared<SolutionPrinter>(...);       // Console output
printer = std::make_shared<HTMLSolutionPrinter>(...);   // HTML format (future)
printer = std::make_shared<JSONSolutionPrinter>(...);   // JSON format (future)

// Client code remains unchanged
printer->print(solution, output);
```

**Polymorphic Behavior**:

```cpp
void processSolutions(ISolutionCollector &collector) {
    // Works with ANY ISolutionCollector implementation
    collector.addSolution(sol);
    if (collector.shouldStop()) return;
}
```

---

### 4. **I**nterface Segregation Principle (ISP)

**Definition**: Clients shouldn't depend on interfaces they don't use.

#### Implementation:

Small, focused interfaces instead of monolithic ones:

**Before (Fat Interface - Violation)**:

```cpp
class ISolver {
    virtual void buildGraph() = 0;
    virtual void solve() = 0;
    virtual void printResults() = 0;
    virtual void exportToFile() = 0;
    virtual void validateInput() = 0;
    // Too many responsibilities in one interface!
};
```

**After (Segregated Interfaces - Compliant)**:

```cpp
// Each interface has a single, focused purpose
class IGraphBuilder {
    virtual void buildGraph(const Grid &grid) = 0;
};

class ISolutionCollector {
    virtual void addSolution(const Solution &solution) = 0;
    virtual bool shouldStop() const = 0;
};

class ISolutionPrinter {
    virtual void print(const Solution &sol, std::ostream &out) const = 0;
    virtual void printSummary(int count, std::ostream &out) const = 0;
};

class IConstraintPropagator {
    virtual bool propagate(State &state) const = 0;
    virtual bool validate(const State &state) const = 0;
};

class IHeuristic {
    virtual int selectNextEdge(const State &state) const = 0;
};
```

**Benefits**:

- Clients only depend on what they actually need
- Easy to mock for testing
- Clear contracts

---

### 5. **D**ependency Inversion Principle (DIP)

**Definition**: Depend on abstractions, not concretions.

#### Implementation:

**Before (Tight Coupling - Violation)**:

```cpp
class Solver {
    SolutionPrinter printer;  // Direct dependency on concrete class

    void run() {
        printer.print(solution);  // Tightly coupled
    }
};
```

**After (Dependency Injection - Compliant)**:

```cpp
class SlitherlinkSolver {
private:
    std::shared_ptr<ISolutionPrinter> solutionPrinter;  // Depend on interface
    std::shared_ptr<ISolutionCollector> solutionCollector;

public:
    SlitherlinkSolver(
        const Grid &g,
        std::shared_ptr<ISolutionCollector> sc,  // Injected dependencies
        std::shared_ptr<ISolutionPrinter> sp
    );
};
```

**Factory-Based Injection**:

```cpp
class SolverFactory {
public:
    static std::unique_ptr<SlitherlinkSolver> createSolver(
        const Grid &grid, bool findAll
    ) {
        // Dependencies are created and injected here
        auto collector = std::make_shared<SolutionCollector>(findAll);
        auto printer = std::make_shared<SolutionPrinter>(...);

        return std::make_unique<SlitherlinkSolver>(
            grid, collector, printer  // Dependency Injection
        );
    }
};
```

**Main Application (Clean)**:

```cpp
int main() {
    Grid grid = readGridFromFile(filename);

    // Depend on abstractions via factory
    auto solver = SolverFactory::createSolver(grid, allSolutions);

    solver->solve();
    solver->printResults(cout);
}
```

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                             │
│                    (Application Layer)                       │
└──────────────────────┬──────────────────────────────────────┘
                       │ depends on
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                    SolverFactory                             │
│              (Creational Pattern - OCP)                      │
└──────────────────────┬──────────────────────────────────────┘
                       │ creates
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                  SlitherlinkSolver                           │
│            (Facade/Orchestrator - SRP, DIP)                  │
└─────┬─────┬─────┬──────────┬─────────┬──────────────────────┘
      │     │     │          │         │
      │     │     │          │         │ depends on (DIP)
      ▼     ▼     ▼          ▼         ▼
    ┌───┐ ┌───┐ ┌───┐     ┌───┐    ┌───┐
    │ I │ │ I │ │ I │     │ I │    │ I │   Interfaces (ISP)
    │ G │ │ C │ │ H │     │ S │    │ S │
    │ r │ │ o │ │ e │     │ o │    │ o │
    │ a │ │ n │ │ u │     │ l │    │ l │
    │ p │ │ s │ │ r │     │ u │    │ u │
    │ h │ │ t │ │ i │     │ t │    │ t │
    │ B │ │ r │ │ s │     │ i │    │ i │
    │ u │ │ a │ │ t │     │ o │    │ o │
    │ i │ │ i │ │ i │     │ n │    │ n │
    │ l │ │ n │ │ c │     │ C │    │ P │
    │ d │ │ t │ │   │     │ o │    │ r │
    │ e │ │   │ │   │     │ l │    │ i │
    │ r │ │   │ │   │     │ l │    │ n │
    │   │ │   │ │   │     │ e │    │ t │
    │   │ │   │ │   │     │ c │    │ e │
    │   │ │   │ │   │     │ t │    │ r │
    │   │ │   │ │   │     │ o │    │   │
    │   │ │   │ │   │     │ r │    │   │
    └─┬─┘ └─┬─┘ └─┬─┘     └─┬─┘    └─┬─┘
      │     │     │         │        │
      ▼     ▼     ▼         ▼        ▼
    ┌───────────────────────────────────┐
    │   Concrete Implementations (LSP)  │
    │  - GraphBuilder                   │
    │  - ConstraintPropagator          │
    │  - EdgeSelectionHeuristic        │
    │  - SolutionCollector             │
    │  - SolutionPrinter               │
    └───────────────────────────────────┘
```

---

## Benefits Achieved

### ✅ Testability

```cpp
// Easy to mock dependencies for unit testing
class MockSolutionCollector : public ISolutionCollector {
    // Test implementation
};

void testSolver() {
    auto mockCollector = std::make_shared<MockSolutionCollector>();
    SlitherlinkSolver solver(grid, mockCollector, ...);
    // Test in isolation
}
```

### ✅ Extensibility

```cpp
// Add new output format without modifying existing code (OCP)
class MarkdownSolutionPrinter : public ISolutionPrinter {
    void print(const Solution &sol, std::ostream &out) const override {
        // Markdown formatting
    }
};
```

### ✅ Maintainability

- Each class has a single, clear purpose (SRP)
- Changes in one area don't affect others
- Easy to locate bugs

### ✅ Flexibility

```cpp
// Swap implementations at runtime
std::shared_ptr<ISolutionPrinter> printer;

if (format == "json")
    printer = std::make_shared<JSONPrinter>(...);
else
    printer = std::make_shared<SolutionPrinter>(...);
```

---

## File Structure (SOLID-Compliant)

```
include/
├── Interfaces (ISP)
│   ├── IGraphBuilder.h
│   ├── IConstraintPropagator.h
│   ├── IHeuristic.h
│   ├── ISolutionCollector.h
│   ├── ISolutionPrinter.h
│   └── ISolutionValidator.h
├── Implementations (SRP)
│   ├── GraphBuilder.h
│   ├── ConstraintPropagator.h
│   ├── EdgeSelectionHeuristic.h
│   ├── SolutionCollector.h
│   └── SolutionPrinter.h
├── Facade (SRP + DIP)
│   └── SlitherlinkSolver.h
└── Factory (OCP)
    └── SolverFactory.h

src/
├── GraphBuilder.cpp
├── SolutionCollector.cpp
├── SolutionPrinter.cpp
├── SlitherlinkSolver.cpp
├── SolverFactory.cpp
└── main.cpp
```

---

## Code Examples

### Before SOLID (Monolithic)

```cpp
struct Solver {
    void run() {
        buildEdges();
        search(state, 0);
        printSolutions();  // Tightly coupled
    }

    void printSolutions() {
        // Direct implementation - hard to test/extend
        for (auto &sol : solutions) {
            cout << sol;  // Fixed output format
        }
    }
};
```

### After SOLID (Decoupled)

```cpp
class SlitherlinkSolver {
private:
    std::shared_ptr<ISolutionPrinter> printer;  // DIP

public:
    SlitherlinkSolver(..., std::shared_ptr<ISolutionPrinter> p)
        : printer(p) {}  // Dependency Injection

    void printResults(std::ostream &out) const {
        printer->printSummary(count, out);  // Delegated, testable
    }
};
```

---

## Testing Strategy

### Unit Tests (Made Possible by SOLID)

```cpp
TEST(GraphBuilderTest, BuildsCorrectGraph) {
    Grid grid = /* ... */;
    GraphBuilder builder;
    builder.buildGraph(grid);
    EXPECT_EQ(builder.getNumPoints(), expectedPoints);
}

TEST(SolutionCollectorTest, StopsAfterFirst) {
    SolutionCollector collector(false);  // findAll = false
    collector.addSolution(solution1);
    EXPECT_TRUE(collector.shouldStop());
}
```

### Integration Tests

```cpp
TEST(SolverIntegrationTest, Solves4x4Puzzle) {
    auto solver = SolverFactory::createSolver(grid, false);
    solver->solve();
    // Verify via injected mock collector
}
```

---

## Future Extensions (Made Easy by SOLID)

### 1. New Output Formats (OCP)

```cpp
class JSONSolutionPrinter : public ISolutionPrinter { /*...*/ };
class HTMLSolutionPrinter : public ISolutionPrinter { /*...*/ };
```

### 2. Alternative Heuristics (OCP + LSP)

```cpp
class RandomSelectionHeuristic : public IHeuristic { /*...*/ };
class AdvancedHeuristic : public IHeuristic { /*...*/ };
```

### 3. Parallel Solution Collection (OCP)

```cpp
class ThreadSafeSolutionCollector : public ISolutionCollector { /*...*/ };
```

### 4. Custom Constraint Propagators (OCP)

```cpp
class OptimizedPropagator : public IConstraintPropagator { /*...*/ };
```

---

## Summary

The SOLID refactoring transforms the codebase from:

- ❌ Monolithic and tightly coupled
- ❌ Hard to test
- ❌ Difficult to extend
- ❌ Mixed responsibilities

To:

- ✅ Modular and decoupled
- ✅ Highly testable
- ✅ Easily extensible
- ✅ Single responsibilities

**Result**: A professional, maintainable, and flexible architecture ready for future development.
