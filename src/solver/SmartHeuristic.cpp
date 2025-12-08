#include "solver/SmartHeuristic.h"
#include <algorithm>
#include <limits>

namespace slitherlink
{

    SmartHeuristic::SmartHeuristic(const Grid &grid,
                                   const std::vector<Edge> &edges,
                                   const std::vector<std::vector<int>> &cellEdges,
                                   int numPoints)
        : grid(grid), edges(edges), cellEdges(cellEdges), numPoints(numPoints)
    {
    }

    int SmartHeuristic::estimateBranches(const State &state, int edgeIdx) const
    {
        const Edge &e = edges[edgeIdx];
        int degU = state.getPointDegree(e.u);
        int degV = state.getPointDegree(e.v);
        int undU = state.getPointUndecided(e.u);
        int undV = state.getPointUndecided(e.v);

        // Quick forced move detection
        if ((degU == 1 && undU == 1) || (degV == 1 && undV == 1))
            return 1; // Must be ON
        if (degU >= 2 || degV >= 2)
            return 1; // Must be OFF

        return 2; // Both branches viable
    }

    int SmartHeuristic::scoreCell(const State &state, int cellIdx) const
    {
        if (cellIdx < 0)
            return 0;

        const std::vector<int> &clues = grid.getClues();
        if (cellIdx >= (int)clues.size() || clues[cellIdx] < 0)
            return 0;

        int clue = clues[cellIdx];
        int cnt = state.getCellEdgeCount(cellIdx);
        int und = state.getCellUndecided(cellIdx);

        if (und == 0)
            return 0;

        int need = clue - cnt;
        if (need == und || need == 0)
            return 2000;
        if (und == 1)
            return 1500;
        if (und <= 2)
            return 1000;

        return std::max(0, 100 - std::abs(need * 2 - und));
    }

    int SmartHeuristic::selectNextEdge(const State &state) const
    {
        int bestEdge = -1;
        int minBranches = 3;
        int bestScore = -1000;

        for (int i = 0; i < (int)edges.size(); ++i)
        {
            if (state.getEdgeState(i) != 0)
                continue;

            // Estimate branching factor
            int branches = estimateBranches(state, i);

            // Forced move - return immediately
            if (branches == 1)
                return i;

            const Edge &e = edges[i];
            int degU = state.getPointDegree(e.u);
            int degV = state.getPointDegree(e.v);
            int undU = state.getPointUndecided(e.u);
            int undV = state.getPointUndecided(e.v);

            int score = ((degU == 1 || degV == 1) ? 10000 : 0) +
                        ((degU == 0 && undU == 2) || (degV == 0 && undV == 2) ? 5000 : 0) +
                        scoreCell(state, e.cellA) + scoreCell(state, e.cellB);

            // Prefer edges with fewer branches, break ties with score
            if (branches < minBranches || (branches == minBranches && score > bestScore))
            {
                minBranches = branches;
                bestScore = score;
                bestEdge = i;
            }
        }

        return bestEdge >= 0 ? bestEdge : (int)edges.size();
    }

} // namespace slitherlink
