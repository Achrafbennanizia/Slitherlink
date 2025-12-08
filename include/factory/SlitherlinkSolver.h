#ifndef SLITHERLINK_FACTORY_SLITHERLINKSOLVER_H
#define SLITHERLINK_FACTORY_SLITHERLINKSOLVER_H

#include "core/Grid.h"
#include "solver/Solver.h"
#include <memory>
#include <iostream>

namespace slitherlink
{

    // Forward declarations
    class ISolutionCollector;
    class ISolutionPrinter;

    /**
     * @brief Main solver facade (SOLID architecture version - not currently used)
     *
     * This is part of the planned SOLID refactoring but not yet integrated.
     * The current system uses the monolithic main.cpp.
     */
    class SlitherlinkSolver
    {
    private:
        Grid grid;
        std::shared_ptr<ISolutionCollector> solutionCollector;
        std::shared_ptr<ISolutionPrinter> solutionPrinter;
        // TODO: Add Solver when SOLID refactoring is complete
        // std::unique_ptr<Solver> solver;

    public:
        SlitherlinkSolver(
            const Grid &g,
            std::shared_ptr<ISolutionCollector> sc,
            std::shared_ptr<ISolutionPrinter> sp);

        void solve();
        void printResults(std::ostream &out = std::cout) const;
    };

} // namespace slitherlink

#endif // SLITHERLINK_FACTORY_SLITHERLINKSOLVER_H
