#include "factory/SlitherlinkSolver.h"
#include "io/SolutionCollector.h"
#include "io/SolutionPrinter.h"
#include <iostream>
namespace slitherlink
{

    SlitherlinkSolver::SlitherlinkSolver(
        const Grid &g,
        std::shared_ptr<ISolutionCollector> sc,
        std::shared_ptr<ISolutionPrinter> sp) : grid(g), solutionCollector(sc), solutionPrinter(sp)
    {
        // NOTE: This SOLID architecture is not fully implemented yet
        // The monolithic main.cpp is currently used instead
        // TODO: Complete Solver integration when refactoring is done
        // solver = std::make_unique<Solver>(...);
    }

    void SlitherlinkSolver::solve()
    {
        // TODO: Implement when Solver API is finalized
        // solver->solve();
    }

    void SlitherlinkSolver::printResults(std::ostream &out) const
    {
        // TODO: Implement when solution collection is integrated
        out << "\nSOLID architecture not yet fully integrated.\n";
        out << "Please use the monolithic executable instead.\n";
    }

} // namespace slitherlink
