#ifndef SLITHERLINK_SMART_HEURISTIC_H
#define SLITHERLINK_SMART_HEURISTIC_H

#include "IHeuristic.h"
#include "Edge.h"
#include "Grid.h"
#include <vector>
#include <memory>

namespace slitherlink
{

    /**
     * @brief Smart heuristic with min-branching factor selection
     *
     * Implements Phase 2 optimization #11
     * Selects edges that minimize search tree branching
     */
    class SmartHeuristic : public IHeuristic
    {
    public:
        SmartHeuristic(const Grid &grid,
                       const std::vector<Edge> &edges,
                       const std::vector<std::vector<int>> &cellEdges,
                       int numPoints);

        int selectNextEdge(const State &state) const override;

    private:
        int estimateBranches(const State &state, int edgeIdx) const;
        int scoreCell(const State &state, int cellIdx) const;

        const Grid &grid;
        const std::vector<Edge> &edges;
        const std::vector<std::vector<int>> &cellEdges;
        int numPoints;
    };

} // namespace slitherlink

#endif // SLITHERLINK_SMART_HEURISTIC_H
