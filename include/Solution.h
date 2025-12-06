#ifndef SLITHERLINK_SOLUTION_H
#define SLITHERLINK_SOLUTION_H

#include <vector>
#include <utility>

namespace slitherlink
{

    /**
     * @brief Represents a complete solution to the puzzle
     *
     * Single Responsibility: Solution data and comparison
     */
    class Solution
    {
    public:
        Solution() = default;

        void setEdgeState(const std::vector<char> &edges) { edgeState = edges; }
        void setCyclePoints(const std::vector<std::pair<int, int>> &points) { cyclePoints = points; }

        const std::vector<char> &getEdgeState() const { return edgeState; }
        const std::vector<std::pair<int, int>> &getCyclePoints() const { return cyclePoints; }

        bool operator<(const Solution &other) const
        {
            return edgeState < other.edgeState;
        }

    private:
        std::vector<char> edgeState;
        std::vector<std::pair<int, int>> cyclePoints;
    };

} // namespace slitherlink

#endif // SLITHERLINK_SOLUTION_H
