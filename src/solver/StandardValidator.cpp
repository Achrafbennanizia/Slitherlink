#include "StandardValidator.h"
#include <stack>
#include <algorithm>

using namespace slitherlink;

bool StandardValidator::quickValidityCheck(const State &state) const
{
    // Check cell constraints
    for (int r = 0; r < grid.getRows(); ++r)
    {
        for (int c = 0; c < grid.getCols(); ++c)
        {
            int cellIdx = grid.cellIndex(r, c);
            int clue = grid.getClue(r, c);
            if (clue < 0)
                continue;

            int onCount = state.getCellEdgeCount(cellIdx);
            int undecided = state.getCellUndecided(cellIdx);

            // Too many ON edges
            if (onCount > clue)
                return false;

            // Not enough possible edges
            if (onCount + undecided < clue)
                return false;
        }
    }

    // Check point degree constraints (max 2 edges per point)
    int numPoints = (grid.getRows() + 1) * (grid.getCols() + 1);
    for (int pt = 0; pt < numPoints; ++pt)
    {
        if (state.getPointDegree(pt) > 2)
            return false;
    }

    return true;
}

bool StandardValidator::isDefinitelyUnsolvable(const State &state) const
{
    // Quick check: any cell constraints violated?
    for (int r = 0; r < grid.getRows(); ++r)
    {
        for (int c = 0; c < grid.getCols(); ++c)
        {
            int cellIdx = grid.cellIndex(r, c);
            int clue = grid.getClue(r, c);
            if (clue < 0)
                continue;

            int onCount = state.getCellEdgeCount(cellIdx);
            int undecided = state.getCellUndecided(cellIdx);
            int maxPossible = onCount + undecided;

            if (onCount > clue || maxPossible < clue)
            {
                return true; // Unsolvable
            }
        }
    }

    // Check for impossible point degrees
    int numPoints = (grid.getRows() + 1) * (grid.getCols() + 1);
    for (int pt = 0; pt < numPoints; ++pt)
    {
        int degree = state.getPointDegree(pt);
        int undecided = state.getPointUndecided(pt);

        // Point has >2 edges already
        if (degree > 2)
            return true;

        // Point has 1 ON edge but no undecided edges left (must have 0 or 2)
        if (degree == 1 && undecided == 0)
            return true;
    }

    return false;
}

bool StandardValidator::hasCycle(const State &state) const
{
    int numPoints = (grid.getRows() + 1) * (grid.getCols() + 1);
    std::vector<std::vector<int>> adj(numPoints);
    int start = -1;

    // Build adjacency list
    for (int pt = 0; pt < numPoints; ++pt)
    {
        adj[pt].reserve(state.getPointDegree(pt));
    }

    int onEdgeCount = 0;
    for (size_t i = 0; i < edges.size(); ++i)
    {
        if (state.getEdgeState(i) == 1)
        {
            const Edge &e = edges[i];
            adj[e.u].push_back(e.v);
            adj[e.v].push_back(e.u);
            if (start == -1)
                start = e.u;
            onEdgeCount++;
        }
    }

    if (start == -1)
        return false; // No ON edges

    // Check all points have degree 0 or 2
    for (int v = 0; v < numPoints; ++v)
    {
        int deg = adj[v].size();
        if (deg != 0 && deg != 2)
            return false;
    }

    // DFS to find single cycle
    std::vector<char> visited(numPoints, 0);
    int visitedEdges = 0;
    std::stack<int> st;
    st.push(start);
    visited[start] = 1;

    while (!st.empty())
    {
        int v = st.top();
        st.pop();

        for (int to : adj[v])
        {
            visitedEdges++;
            if (!visited[to])
            {
                visited[to] = 1;
                st.push(to);
            }
        }
    }

    // Check all points with edges were visited (single component)
    for (int v = 0; v < numPoints; ++v)
    {
        if (!adj[v].empty() && !visited[v])
        {
            return false; // Multiple components
        }
    }

    // visitedEdges counts each edge twice (u->v and v->u)
    return (visitedEdges / 2 == onEdgeCount);
}

bool StandardValidator::checkCellConstraints(const State &state) const
{
    for (int r = 0; r < grid.getRows(); ++r)
    {
        for (int c = 0; c < grid.getCols(); ++c)
        {
            int cellIdx = grid.cellIndex(r, c);
            int clue = grid.getClue(r, c);
            if (clue < 0)
                continue;

            int onCount = state.getCellEdgeCount(cellIdx);
            if (onCount != clue)
            {
                return false;
            }
        }
    }
    return true;
}

bool StandardValidator::isValid(const State &state) const
{
    // A complete solution must:
    // 1. Have all cell clue constraints satisfied
    if (!checkCellConstraints(state))
        return false;

    // 2. Form a single cycle
    if (!hasCycle(state))
        return false;

    return true;
}

bool StandardValidator::isUnsolvable(const State &state) const
{
    return isDefinitelyUnsolvable(state);
}
