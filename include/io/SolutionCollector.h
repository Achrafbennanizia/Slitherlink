#ifndef SLITHERLINK_IO_SOLUTIONCOLLECTOR_H
#define SLITHERLINK_IO_SOLUTIONCOLLECTOR_H

#include "core/Solution.h"
#include <vector>
#include <memory>

namespace slitherlink
{

    /**
     * @brief Interface for collecting solutions
     */
    class ISolutionCollector
    {
    public:
        virtual ~ISolutionCollector() = default;
        virtual void addSolution(const Solution &sol) = 0;
        virtual const std::vector<Solution> &getSolutions() const = 0;
        virtual bool shouldContinue() const = 0;
    };

    /**
     * @brief Collects solver solutions (SOLID architecture)
     *
     * Part of planned SOLID refactoring - not currently used.
     */
    class SolutionCollector : public ISolutionCollector
    {
    private:
        std::vector<Solution> solutions;
        bool findAll;

    public:
        explicit SolutionCollector(bool findAllSolutions = false);

        void addSolution(const Solution &sol) override;
        const std::vector<Solution> &getSolutions() const override;
        bool shouldContinue() const override;
    };

} // namespace slitherlink

#endif // SLITHERLINK_IO_SOLUTIONCOLLECTOR_H
