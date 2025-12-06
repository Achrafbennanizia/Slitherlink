#include "slitherlink/factory/SlitherlinkSolver.h"
#include <iostream>
namespace slitherlink {

SlitherlinkSolver::SlitherlinkSolver(
    const Grid &g,
    std::shared_ptr<ISolutionCollector> sc,
    std::shared_ptr<ISolutionPrinter> sp
) : grid(g), solutionCollector(sc), solutionPrinter(sp)
{
    solver = std::make_unique<Solver>();
    solver->grid = grid;
}

void SlitherlinkSolver::solve()
{
    solver->run(false); // Find first solution by default
}

void SlitherlinkSolver::printResults(std::ostream &out) const
{
    const auto &solutions = solver->solutions;
    
    if (solutions.empty())
    {
        out << "\nNo solutions found.\n";
        return;
    }
    
    // Print summary using injected printer (SOLID: Dependency Inversion)
    solutionPrinter->printSummary(solutions.size(), out);
}

} // namespace slitherlink
