#ifndef SOLVER_H
#define SOLVER_H

#include "Grid.h"
#include "Edge.h"
#include "State.h"
#include "Solution.h"
#include "IHeuristic.h"
#include "IValidator.h"
#include "IPropagator.h"
#include <vector>
#include <memory>
#include <atomic>

namespace slitherlink
{

    struct SolverConfig
    {
        int threads = 1;
        bool findAll = false;
        bool verbose = false;
        bool enableParallel = true;
    };

    class Solver
    {
    private:
        const Grid &grid;
        const std::vector<Edge> &edges;
        const std::vector<std::vector<int>> &adjacentEdges;
        const std::vector<std::vector<int>> &pointEdges;

        // Dependency injection - depend on abstractions, not concretions
        std::unique_ptr<IHeuristic> heuristic;
        std::unique_ptr<IValidator> validator;
        std::unique_ptr<IPropagator> propagator;

        SolverConfig config;

        // Solution tracking
        mutable std::vector<Solution> solutions;
        mutable std::atomic<bool> foundSolution{false};

        // Search functions
        void search(State &state);
        void parallelSearch(State &initialState);
        bool extractSolution(const State &state, Solution &sol) const;

    public:
        Solver(const Grid &g,
               const std::vector<Edge> &e,
               const std::vector<std::vector<int>> &adjEdges,
               const std::vector<std::vector<int>> &ptEdges,
               std::unique_ptr<IHeuristic> h,
               std::unique_ptr<IValidator> v,
               std::unique_ptr<IPropagator> p,
               const SolverConfig &cfg = SolverConfig())
            : grid(g), edges(e), adjacentEdges(adjEdges), pointEdges(ptEdges),
              heuristic(std::move(h)), validator(std::move(v)), propagator(std::move(p)),
              config(cfg) {}

        std::vector<Solution> solve();
        const std::vector<Solution> &getSolutions() const { return solutions; }
    };

} // namespace slitherlink

#endif // SOLVER_H
