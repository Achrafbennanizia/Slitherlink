#ifndef SLITHERLINK_FACTORY_SOLVERFACTORY_H
#define SLITHERLINK_FACTORY_SOLVERFACTORY_H

#include "core/Grid.h"
#include "factory/SlitherlinkSolver.h"
#include <memory>

namespace slitherlink
{

    /**
     * @brief Factory for creating SlitherlinkSolver instances (SOLID architecture)
     *
     * Part of planned SOLID refactoring - not currently used in production.
     */
    class SolverFactory
    {
    public:
        static std::unique_ptr<SlitherlinkSolver> createSolver(const Grid &grid, bool findAll = false);
    };

} // namespace slitherlink

#endif // SLITHERLINK_FACTORY_SOLVERFACTORY_H
