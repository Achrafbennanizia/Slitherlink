#include "slitherlink/io/SolutionCollector.h"
#include <iostream>
namespace slitherlink {

void SolutionCollector::addSolution(const Solution &solution)
{
    std::lock_guard<std::mutex> lock(mutex);

    int solNum = ++count;
    std::cout << "\n=== Solution " << solNum << " found! ===\n"
              << std::flush;

    solutions.push_back(solution);

    if (!findAll)
    {
        stopRequested.store(true, std::memory_order_relaxed);
    }
}

} // namespace slitherlink
