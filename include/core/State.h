#ifndef SLITHERLINK_STATE_H
#define SLITHERLINK_STATE_H

#include <vector>

namespace slitherlink
{

    /**
     * @brief Represents the current state of the search
     *
     * Single Responsibility: State management and data storage
     * Cache-friendly layout with frequently accessed data grouped together
     */
    class State
    {
    public:
        State() = default;
        State(const State &) = default;
        State(State &&) noexcept = default;
        State &operator=(const State &) = default;
        State &operator=(State &&) noexcept = default;

        // Accessors
        char getEdgeState(int idx) const { return edgeState[idx]; }
        void setEdgeState(int idx, char val) { edgeState[idx] = val; }

        int getPointDegree(int idx) const { return pointDegree[idx]; }
        void setPointDegree(int idx, int val) { pointDegree[idx] = val; }
        void incrementPointDegree(int idx) { pointDegree[idx]++; }

        int getPointUndecided(int idx) const { return pointUndecided[idx]; }
        void setPointUndecided(int idx, int val) { pointUndecided[idx] = val; }
        void decrementPointUndecided(int idx) { pointUndecided[idx]--; }

        int getCellEdgeCount(int idx) const { return cellEdgeCount[idx]; }
        void setCellEdgeCount(int idx, int val) { cellEdgeCount[idx] = val; }
        void incrementCellEdgeCount(int idx) { cellEdgeCount[idx]++; }

        int getCellUndecided(int idx) const { return cellUndecided[idx]; }
        void setCellUndecided(int idx, int val) { cellUndecided[idx] = val; }
        void decrementCellUndecided(int idx) { cellUndecided[idx]--; }

        // Direct access for performance-critical code
        std::vector<char> &getEdgeStateVector() { return edgeState; }
        const std::vector<char> &getEdgeStateVector() const { return edgeState; }

        // Initialization
        void initialize(size_t edgeCount, size_t pointCount, size_t cellCount);

    private:
        // Cache-friendly layout: group frequently accessed data together
        std::vector<char> edgeState;     ///< 0=undecided, 1=ON, -1=OFF
        std::vector<int> pointDegree;    ///< Number of ON edges at each point
        std::vector<int> pointUndecided; ///< Number of undecided edges per point
        std::vector<int> cellEdgeCount;  ///< Number of ON edges around each cell
        std::vector<int> cellUndecided;  ///< Number of undecided edges per cell
    };

} // namespace slitherlink

#endif // SLITHERLINK_STATE_H
