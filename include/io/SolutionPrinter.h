#ifndef SLITHERLINK_IO_SOLUTIONPRINTER_H
#define SLITHERLINK_IO_SOLUTIONPRINTER_H

#include "core/Grid.h"
#include "core/Solution.h"
#include <iostream>
#include <vector>
#include <map>

namespace slitherlink
{

    /**
     * @brief Interface for printing solutions
     */
    class ISolutionPrinter
    {
    public:
        virtual ~ISolutionPrinter() = default;
        virtual void printSolution(const Solution &sol, std::ostream &out = std::cout) const = 0;
        virtual void printSummary(size_t count, std::ostream &out = std::cout) const = 0;
    };

    /**
     * @brief Prints solver solutions (SOLID architecture)
     *
     * Part of planned SOLID refactoring - not currently used.
     */
    class SolutionPrinter : public ISolutionPrinter
    {
    private:
        Grid grid;
        std::map<std::pair<int, int>, int> horizEdgeIndex;
        std::map<std::pair<int, int>, int> vertEdgeIndex;

    public:
        SolutionPrinter(
            const Grid &g,
            const std::map<std::pair<int, int>, int> &hIndex,
            const std::map<std::pair<int, int>, int> &vIndex);

        void printSolution(const Solution &sol, std::ostream &out = std::cout) const override;
        void printSummary(size_t count, std::ostream &out = std::cout) const override;
    };

} // namespace slitherlink

#endif // SLITHERLINK_IO_SOLUTIONPRINTER_H
