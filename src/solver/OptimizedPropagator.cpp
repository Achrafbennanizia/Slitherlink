#include "solver/OptimizedPropagator.h"

using namespace slitherlink;

bool OptimizedPropagator::propagateCell(State &state, int cellIdx) const
{
    int clue = grid.getClue(cellIdx / grid.getCols(), cellIdx % grid.getCols());
    if (clue < 0)
        return true;

    int onCount = state.getCellEdgeCount(cellIdx);
    int undecided = state.getCellUndecided(cellIdx);

    // Check validity
    if (onCount > clue || onCount + undecided < clue)
    {
        return false;
    }

    return true;
}

bool OptimizedPropagator::propagatePoint(State &state, int pointIdx) const
{
    int degree = state.getPointDegree(pointIdx);
    int undecided = state.getPointUndecided(pointIdx);

    // Points can have at most 2 edges
    if (degree > 2)
        return false;

    // Points with 1 edge must have another (can't have odd degree in a cycle)
    if (degree == 1 && undecided == 0)
        return false;

    return true;
}

bool OptimizedPropagator::propagateEdge(State &state, int edgeIdx) const
{
    // Edge propagation handled by cell and point constraints
    return true;
}

bool OptimizedPropagator::propagate(State &state) const
{
    // Initial validation
    int numCells = grid.getRows() * grid.getCols();
    int numPoints = (grid.getRows() + 1) * (grid.getCols() + 1);

    for (int cell = 0; cell < numCells; ++cell)
    {
        int clue = grid.getClue(cell / grid.getCols(), cell % grid.getCols());
        if (clue < 0)
            continue;

        int onCount = state.getCellEdgeCount(cell);
        int undecided = state.getCellUndecided(cell);
        int maxPossible = onCount + undecided;

        if (onCount > clue || maxPossible < clue)
        {
            return false;
        }
    }

    // Phase 2 optimization: Use uint8_t queues for cache efficiency
    cellQueue.clear();
    pointQueue.clear();
    edgeQueue.clear();

    std::vector<uint8_t> cellQueued(numCells, 0);
    std::vector<uint8_t> pointQueued(numPoints, 0);

    // Initialize queues with all cells that have clues
    for (int r = 0; r < grid.getRows(); ++r)
    {
        for (int c = 0; c < grid.getCols(); ++c)
        {
            int cellIdx = grid.cellIndex(r, c);
            if (grid.getClue(r, c) >= 0)
            {
                cellQueue.push_back(cellIdx);
                cellQueued[cellIdx] = 1;
            }
        }
    }

    // Initialize with all points
    for (int i = 0; i < numPoints; ++i)
    {
        pointQueue.push_back(i);
        pointQueued[i] = 1;
    }

    size_t cellPos = 0, pointPos = 0;

    // Propagation loop
    while (cellPos < cellQueue.size() || pointPos < pointQueue.size())
    {
        // Process cells
        while (cellPos < cellQueue.size())
        {
            int cellIdx = cellQueue[cellPos++];
            cellQueued[cellIdx] = 0;

            int clue = grid.getClue(cellIdx / grid.getCols(), cellIdx % grid.getCols());
            if (clue < 0)
                continue;

            int onCount = state.getCellEdgeCount(cellIdx);
            int undecided = state.getCellUndecided(cellIdx);

            // If all remaining edges must be ON
            if (onCount + undecided == clue)
            {
                for (int eidx : adjacentEdges[cellIdx])
                {
                    if (state.getEdgeState(eidx) == 0)
                    {
                        if (!applyDecision(state, eidx, 1))
                        {
                            return false;
                        }

                        // Queue affected cells and points
                        const Edge &e = edges[eidx];
                        if (e.cellA >= 0 && !cellQueued[e.cellA])
                        {
                            if (grid.getClue(e.cellA / grid.getCols(), e.cellA % grid.getCols()) >= 0)
                            {
                                cellQueue.push_back(e.cellA);
                                cellQueued[e.cellA] = 1;
                            }
                        }
                        if (e.cellB >= 0 && !cellQueued[e.cellB])
                        {
                            if (grid.getClue(e.cellB / grid.getCols(), e.cellB % grid.getCols()) >= 0)
                            {
                                cellQueue.push_back(e.cellB);
                                cellQueued[e.cellB] = 1;
                            }
                        }
                        if (!pointQueued[e.u])
                        {
                            pointQueue.push_back(e.u);
                            pointQueued[e.u] = 1;
                        }
                        if (!pointQueued[e.v])
                        {
                            pointQueue.push_back(e.v);
                            pointQueued[e.v] = 1;
                        }
                    }
                }
            }
            // If clue is satisfied, turn off remaining edges
            else if (onCount == clue && undecided > 0)
            {
                for (int eidx : adjacentEdges[cellIdx])
                {
                    if (state.getEdgeState(eidx) == 0)
                    {
                        state.setEdgeState(eidx, -1);
                        const Edge &e = edges[eidx];
                        state.decrementPointUndecided(e.u);
                        state.decrementPointUndecided(e.v);
                        if (e.cellA >= 0)
                            state.decrementCellUndecided(e.cellA);
                        if (e.cellB >= 0)
                            state.decrementCellUndecided(e.cellB);

                        if (!pointQueued[e.u])
                        {
                            pointQueue.push_back(e.u);
                            pointQueued[e.u] = 1;
                        }
                        if (!pointQueued[e.v])
                        {
                            pointQueue.push_back(e.v);
                            pointQueued[e.v] = 1;
                        }
                    }
                }
            }
        }

        // Process points
        while (pointPos < pointQueue.size())
        {
            int ptIdx = pointQueue[pointPos++];
            pointQueued[ptIdx] = 0;

            int deg = state.getPointDegree(ptIdx);
            int undecided = state.getPointUndecided(ptIdx);

            // Skip if nothing to propagate
            if (deg >= 2 || (deg == 0 && undecided == 0))
            {
                continue;
            }

            // Point has 1 ON edge and 1 undecided: must turn ON the undecided
            if (deg == 1 && undecided == 1)
            {
                for (int eidx : pointEdges[ptIdx])
                {
                    if (state.getEdgeState(eidx) == 0)
                    {
                        if (!applyDecision(state, eidx, 1))
                        {
                            return false;
                        }

                        const Edge &e = edges[eidx];
                        if (e.cellA >= 0 && !cellQueued[e.cellA])
                        {
                            if (grid.getClue(e.cellA / grid.getCols(), e.cellA % grid.getCols()) >= 0)
                            {
                                cellQueue.push_back(e.cellA);
                                cellQueued[e.cellA] = 1;
                            }
                        }
                        if (e.cellB >= 0 && !cellQueued[e.cellB])
                        {
                            if (grid.getClue(e.cellB / grid.getCols(), e.cellB % grid.getCols()) >= 0)
                            {
                                cellQueue.push_back(e.cellB);
                                cellQueued[e.cellB] = 1;
                            }
                        }
                        int otherPt = (e.u == ptIdx) ? e.v : e.u;
                        if (!pointQueued[otherPt])
                        {
                            pointQueue.push_back(otherPt);
                            pointQueued[otherPt] = 1;
                        }
                    }
                }
            }
            // Point already has 2 ON edges: turn off all remaining
            else if (deg == 2 && undecided > 0)
            {
                for (int eidx : pointEdges[ptIdx])
                {
                    if (state.getEdgeState(eidx) == 0)
                    {
                        state.setEdgeState(eidx, -1);
                        const Edge &e = edges[eidx];
                        state.decrementPointUndecided(e.u);
                        state.decrementPointUndecided(e.v);
                        if (e.cellA >= 0)
                            state.decrementCellUndecided(e.cellA);
                        if (e.cellB >= 0)
                            state.decrementCellUndecided(e.cellB);

                        int otherPt = (e.u == ptIdx) ? e.v : e.u;
                        if (!pointQueued[otherPt])
                        {
                            pointQueue.push_back(otherPt);
                            pointQueued[otherPt] = 1;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool OptimizedPropagator::applyDecision(State &state, int edgeIdx, int value) const
{
    state.setEdgeState(edgeIdx, value);
    const Edge &e = edges[edgeIdx];

    if (value == 1)
    {
        // Turn ON
        state.incrementPointDegree(e.u);
        state.incrementPointDegree(e.v);
        state.decrementPointUndecided(e.u);
        state.decrementPointUndecided(e.v);

        if (e.cellA >= 0)
        {
            state.incrementCellEdgeCount(e.cellA);
            state.decrementCellUndecided(e.cellA);
        }
        if (e.cellB >= 0)
        {
            state.incrementCellEdgeCount(e.cellB);
            state.decrementCellUndecided(e.cellB);
        }

        // Check constraints
        if (state.getPointDegree(e.u) > 2 || state.getPointDegree(e.v) > 2)
        {
            return false;
        }
        if (e.cellA >= 0)
        {
            int clue = grid.getClue(e.cellA / grid.getCols(), e.cellA % grid.getCols());
            if (clue >= 0 && state.getCellEdgeCount(e.cellA) > clue)
            {
                return false;
            }
        }
        if (e.cellB >= 0)
        {
            int clue = grid.getClue(e.cellB / grid.getCols(), e.cellB % grid.getCols());
            if (clue >= 0 && state.getCellEdgeCount(e.cellB) > clue)
            {
                return false;
            }
        }
    }
    else
    {
        // Turn OFF (value == -1)
        state.decrementPointUndecided(e.u);
        state.decrementPointUndecided(e.v);
        if (e.cellA >= 0)
            state.decrementCellUndecided(e.cellA);
        if (e.cellB >= 0)
            state.decrementCellUndecided(e.cellB);
    }

    return true;
}
