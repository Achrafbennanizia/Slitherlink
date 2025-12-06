#include "slitherlink/solver/GraphBuilder.h"
namespace slitherlink {

void GraphBuilder::buildGraph(const Grid &grid)
{
    int n = grid.n, m = grid.m;
    numPoints = (n + 1) * (m + 1);
    horizEdgeIndex.assign((n + 1) * m, -1);
    vertEdgeIndex.assign(n * (m + 1), -1);
    cellEdges.assign(n * m, {});
    pointEdges.assign(numPoints, {});
    edges.clear();
    clueCells.clear();
    clueCells.reserve(grid.clues.size());

    auto pointId = [m](int r, int c)
    { return r * (m + 1) + c; };
    int idx = 0;

    for (int r = 0; r <= n; ++r)
        for (int c = 0; c < m; ++c)
        {
            Edge e{pointId(r, c), pointId(r, c + 1),
                   (r > 0) ? grid.cellIndex(r - 1, c) : -1,
                   (r < n) ? grid.cellIndex(r, c) : -1};
            edges.push_back(e);
            horizEdgeIndex[r * m + c] = idx;
            if (e.cellA >= 0)
                cellEdges[e.cellA].push_back(idx);
            if (e.cellB >= 0)
                cellEdges[e.cellB].push_back(idx);
            pointEdges[e.u].push_back(idx);
            pointEdges[e.v].push_back(idx);
            idx++;
        }

    for (int r = 0; r < n; ++r)
        for (int c = 0; c <= m; ++c)
        {
            Edge e{pointId(r, c), pointId(r + 1, c),
                   (c > 0) ? grid.cellIndex(r, c - 1) : -1,
                   (c < m) ? grid.cellIndex(r, c) : -1};
            edges.push_back(e);
            vertEdgeIndex[r * (m + 1) + c] = idx;
            if (e.cellA >= 0)
                cellEdges[e.cellA].push_back(idx);
            if (e.cellB >= 0)
                cellEdges[e.cellB].push_back(idx);
            pointEdges[e.u].push_back(idx);
            pointEdges[e.v].push_back(idx);
            idx++;
        }

    for (size_t i = 0; i < grid.clues.size(); ++i)
        if (grid.clues[i] >= 0)
            clueCells.push_back((int)i);
}

} // namespace slitherlink
