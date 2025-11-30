#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <stack>
#include <mutex>
#include <atomic>
#include <future>
#include <chrono>
#include <memory>
#include <queue>
#include <thread>
#include <condition_variable>
#include <algorithm>
#include <numeric>

#ifdef USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>
#include <tbb/parallel_reduce.h>
#include <tbb/parallel_scan.h>
#include <tbb/blocked_range.h>
#include <tbb/task_arena.h>
#include <tbb/task_group.h>
#include <tbb/global_control.h>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_queue.h>
#include <tbb/spin_mutex.h>
#endif

using namespace std;

struct Grid
{
    int n = 0, m = 0;
    vector<int> clues; // size n*m, -1 for none

    int cellIndex(int r, int c) const { return r * m + c; }
};

struct Edge
{
    int u, v;  // endpoints (point indices)
    int cellA; // adjacent cell index or -1
    int cellB; // second adjacent cell index or -1
};

struct State
{
    vector<char> edgeState;     // 0 undecided, 1 on, -1 off
    vector<int> pointDegree;    // degree of each point from ON edges
    vector<int> cellEdgeCount;  // count ON edges around each cell
    vector<int> cellUndecided;  // count undecided edges per cell
    vector<int> pointUndecided; // count undecided edges per point

    State() = default;
    State(const State &) = default;
    State(State &&) noexcept = default;
    State &operator=(const State &) = default;
    State &operator=(State &&) noexcept = default;
};

struct Solution
{
    vector<char> edgeState;
    vector<pair<int, int>> cyclePoints; // ordered cycle as (row,col) on point grid
};

struct Solver
{
    Grid grid;
    vector<Edge> edges;
    int numPoints = 0;

    vector<int> horizEdgeIndex;     // (n+1)*m
    vector<int> vertEdgeIndex;      // n*(m+1)
    vector<vector<int>> cellEdges;  // edges adjacent to each cell
    vector<vector<int>> pointEdges; // edges adjacent to each point

    bool findAll = false;
    atomic<bool> stopAfterFirst{false};

    mutex solMutex;
    vector<Solution> solutions;
    atomic<int> solutionCount{0};

    int maxParallelDepth = 8;

#ifdef USE_TBB
    unique_ptr<tbb::task_arena> arena;
    tbb::concurrent_vector<Solution> tbbSolutions;
#endif

    void buildEdges()
    {
        int n = grid.n, m = grid.m;
        numPoints = (n + 1) * (m + 1);
        horizEdgeIndex.assign((n + 1) * m, -1);
        vertEdgeIndex.assign(n * (m + 1), -1);
        cellEdges.assign(n * m, {});
        pointEdges.assign(numPoints, {});
        edges.clear();

        auto pointId = [m](int r, int c) { return r * (m + 1) + c; };
        int idx = 0;

        // Build horizontal edges
        for (int r = 0; r <= n; ++r)
            for (int c = 0; c < m; ++c)
            {
                Edge e{pointId(r, c), pointId(r, c + 1),
                       (r > 0) ? grid.cellIndex(r - 1, c) : -1,
                       (r < n) ? grid.cellIndex(r, c) : -1};
                edges.push_back(e);
                horizEdgeIndex[r * m + c] = idx;
                if (e.cellA >= 0) cellEdges[e.cellA].push_back(idx);
                if (e.cellB >= 0) cellEdges[e.cellB].push_back(idx);
                pointEdges[e.u].push_back(idx);
                pointEdges[e.v].push_back(idx);
                idx++;
            }

        // Build vertical edges
        for (int r = 0; r < n; ++r)
            for (int c = 0; c <= m; ++c)
            {
                Edge e{pointId(r, c), pointId(r + 1, c),
                       (c > 0) ? grid.cellIndex(r, c - 1) : -1,
                       (c < m) ? grid.cellIndex(r, c) : -1};
                edges.push_back(e);
                vertEdgeIndex[r * (m + 1) + c] = idx;
                if (e.cellA >= 0) cellEdges[e.cellA].push_back(idx);
                if (e.cellB >= 0) cellEdges[e.cellB].push_back(idx);
                pointEdges[e.u].push_back(idx);
                pointEdges[e.v].push_back(idx);
                idx++;
            }
    }

    State initialState() const
    {
        State s;
        s.edgeState.assign(edges.size(), 0);
        s.pointDegree.assign(numPoints, 0);
        s.cellEdgeCount.assign(grid.clues.size(), 0);
        s.cellUndecided.assign(grid.clues.size(), 0);
        s.pointUndecided.assign(numPoints, 0);

        // Initialize undecided counts
#ifdef USE_TBB
        vector<size_t> cellIndices(cellEdges.size());
        iota(cellIndices.begin(), cellIndices.end(), 0);
        tbb::parallel_for_each(cellIndices.begin(), cellIndices.end(),
                               [&](size_t idx)
                               {
                                   s.cellUndecided[idx] = (int)cellEdges[idx].size();
                               });

        vector<int> pointIndices(numPoints);
        iota(pointIndices.begin(), pointIndices.end(), 0);
        tbb::parallel_for_each(pointIndices.begin(), pointIndices.end(),
                               [&](int idx)
                               {
                                   s.pointUndecided[idx] = (int)pointEdges[idx].size();
                               });
#else
        for (size_t i = 0; i < cellEdges.size(); ++i)
            s.cellUndecided[i] = (int)cellEdges[i].size();
        for (int i = 0; i < numPoints; ++i)
            s.pointUndecided[i] = (int)pointEdges[i].size();
#endif

        return s;
    }

    bool applyDecision(State &s, int edgeIdx, int val) const
    {
        if (s.edgeState[edgeIdx] == val)
            return true;
        if (s.edgeState[edgeIdx] != 0)
            return false;

        s.edgeState[edgeIdx] = (char)val;
        const Edge &e = edges[edgeIdx];

        // Update undecided counts
        s.pointUndecided[e.u]--;
        s.pointUndecided[e.v]--;
        if (e.cellA >= 0)
            s.cellUndecided[e.cellA]--;
        if (e.cellB >= 0)
            s.cellUndecided[e.cellB]--;

        if (val == 1)
        {
            int du = ++s.pointDegree[e.u];
            int dv = ++s.pointDegree[e.v];
            if (du > 2 || dv > 2)
                return false;

            if (e.cellA >= 0)
            {
                int cnt = ++s.cellEdgeCount[e.cellA];
                if (grid.clues[e.cellA] >= 0 && cnt > grid.clues[e.cellA])
                    return false;
            }
            if (e.cellB >= 0)
            {
                int cnt = ++s.cellEdgeCount[e.cellB];
                if (grid.clues[e.cellB] >= 0 && cnt > grid.clues[e.cellB])
                    return false;
            }
        }
        return true;
    }

    bool quickValidityCheck(const State &s) const
    {
#ifdef USE_TBB
        bool pointsOk = tbb::parallel_reduce(
            tbb::blocked_range<int>(0, numPoints),
            true,
            [&](const tbb::blocked_range<int> &r, bool ok) -> bool
            {
                if (!ok)
                    return false;
                for (int i = r.begin(); i != r.end(); ++i)
                {
                    if (s.pointDegree[i] > 2)
                        return false;
                    if (s.pointDegree[i] == 1 && s.pointUndecided[i] == 0)
                        return false;
                }
                return true;
            },
            [](bool a, bool b)
            { return a && b; });
        if (!pointsOk)
            return false;

        bool cellsOk = tbb::parallel_reduce(
            tbb::blocked_range<size_t>(0, grid.clues.size()),
            true,
            [&](const tbb::blocked_range<size_t> &r, bool ok) -> bool
            {
                if (!ok)
                    return false;
                for (size_t i = r.begin(); i != r.end(); ++i)
                {
                    int clue = grid.clues[i];
                    if (clue < 0)
                        continue;
                    if (s.cellEdgeCount[i] > clue)
                        return false;
                    if (s.cellEdgeCount[i] + s.cellUndecided[i] < clue)
                        return false;
                }
                return true;
            },
            [](bool a, bool b)
            { return a && b; });
        return cellsOk;
#else
        // Quick check: no point should have degree > 2
        for (int i = 0; i < numPoints; ++i)
        {
            if (s.pointDegree[i] > 2)
                return false;
            // If degree is 1 and no undecided edges, invalid
            if (s.pointDegree[i] == 1 && s.pointUndecided[i] == 0)
                return false;
        }

        // Quick check: cells with clues shouldn't exceed their limit
        for (size_t i = 0; i < grid.clues.size(); ++i)
        {
            if (grid.clues[i] >= 0)
            {
                if (s.cellEdgeCount[i] > grid.clues[i])
                    return false;
                // If we can't reach the clue even with all undecided edges
                if (s.cellEdgeCount[i] + s.cellUndecided[i] < grid.clues[i])
                    return false;
            }
        }
        return true;
#endif
    }

    bool propagateConstraints(State &s) const
    {
        // Early pruning: check for impossible cell states
        for (size_t i = 0; i < grid.clues.size(); ++i)
        {
            int clue = grid.clues[i];
            if (clue < 0)
                continue;

            int onCount = s.cellEdgeCount[i];
            int undecided = s.cellUndecided[i];
            int maxPossible = onCount + undecided;

            // Impossible to satisfy constraint
            if (onCount > clue || maxPossible < clue)
                return false;
        }

        vector<int> cellQueue, pointQueue;
        vector<bool> cellQueued(grid.clues.size(), false);
        vector<bool> pointQueued(numPoints, false);

        // Initial queue: all cells and points
        for (size_t i = 0; i < grid.clues.size(); ++i)
            if (grid.clues[i] >= 0)
            {
                cellQueue.push_back(i);
                cellQueued[i] = true;
            }
        for (int i = 0; i < numPoints; ++i)
        {
            pointQueue.push_back(i);
            pointQueued[i] = true;
        }

        size_t cellPos = 0, pointPos = 0;

        while (cellPos < cellQueue.size() || pointPos < pointQueue.size())
        {
            // Process cells
            while (cellPos < cellQueue.size())
            {
                int cellIdx = cellQueue[cellPos++];
                cellQueued[cellIdx] = false;

                int clue = grid.clues[cellIdx];
                if (clue < 0)
                    continue;

                int onCount = s.cellEdgeCount[cellIdx];
                int undecided = s.cellUndecided[cellIdx];

                // If we need all remaining edges
                if (onCount + undecided == clue)
                {
                    for (int eidx : cellEdges[cellIdx])
                    {
                        if (s.edgeState[eidx] == 0)
                        {
                            if (!applyDecision(s, eidx, 1))
                                return false;

                            // Queue affected cells and points
                            const Edge &e = edges[eidx];
                            if (e.cellA >= 0 && !cellQueued[e.cellA] && grid.clues[e.cellA] >= 0)
                            {
                                cellQueue.push_back(e.cellA);
                                cellQueued[e.cellA] = true;
                            }
                            if (e.cellB >= 0 && !cellQueued[e.cellB] && grid.clues[e.cellB] >= 0)
                            {
                                cellQueue.push_back(e.cellB);
                                cellQueued[e.cellB] = true;
                            }
                            if (!pointQueued[e.u])
                            {
                                pointQueue.push_back(e.u);
                                pointQueued[e.u] = true;
                            }
                            if (!pointQueued[e.v])
                            {
                                pointQueue.push_back(e.v);
                                pointQueued[e.v] = true;
                            }
                        }
                    }
                }
                // If we have enough edges, turn off rest
                else if (onCount == clue && undecided > 0)
                {
                    for (int eidx : cellEdges[cellIdx])
                    {
                        if (s.edgeState[eidx] == 0)
                        {
                            s.edgeState[eidx] = -1;
                            const Edge &e = edges[eidx];
                            s.pointUndecided[e.u]--;
                            s.pointUndecided[e.v]--;
                            if (e.cellA >= 0)
                                s.cellUndecided[e.cellA]--;
                            if (e.cellB >= 0)
                                s.cellUndecided[e.cellB]--;

                            if (!pointQueued[e.u])
                            {
                                pointQueue.push_back(e.u);
                                pointQueued[e.u] = true;
                            }
                            if (!pointQueued[e.v])
                            {
                                pointQueue.push_back(e.v);
                                pointQueued[e.v] = true;
                            }
                        }
                    }
                }
            }

            // Process points
            while (pointPos < pointQueue.size())
            {
                int ptIdx = pointQueue[pointPos++];
                pointQueued[ptIdx] = false;

                int deg = s.pointDegree[ptIdx];
                int undecided = s.pointUndecided[ptIdx];

                if (deg >= 2 || (deg == 0 && undecided == 0))
                    continue;

                // If degree is 1, need exactly 1 more
                if (deg == 1 && undecided == 1)
                {
                    for (int eidx : pointEdges[ptIdx])
                    {
                        if (s.edgeState[eidx] == 0)
                        {
                            if (!applyDecision(s, eidx, 1))
                                return false;

                            const Edge &e = edges[eidx];
                            if (e.cellA >= 0 && !cellQueued[e.cellA] && grid.clues[e.cellA] >= 0)
                            {
                                cellQueue.push_back(e.cellA);
                                cellQueued[e.cellA] = true;
                            }
                            if (e.cellB >= 0 && !cellQueued[e.cellB] && grid.clues[e.cellB] >= 0)
                            {
                                cellQueue.push_back(e.cellB);
                                cellQueued[e.cellB] = true;
                            }
                            int otherPt = (e.u == ptIdx) ? e.v : e.u;
                            if (!pointQueued[otherPt])
                            {
                                pointQueue.push_back(otherPt);
                                pointQueued[otherPt] = true;
                            }
                        }
                    }
                }
                // If degree is 2, turn off remaining
                else if (deg == 2 && undecided > 0)
                {
                    for (int eidx : pointEdges[ptIdx])
                    {
                        if (s.edgeState[eidx] == 0)
                        {
                            s.edgeState[eidx] = -1;
                            const Edge &e = edges[eidx];
                            s.pointUndecided[e.u]--;
                            s.pointUndecided[e.v]--;
                            if (e.cellA >= 0)
                                s.cellUndecided[e.cellA]--;
                            if (e.cellB >= 0)
                                s.cellUndecided[e.cellB]--;

                            int otherPt = (e.u == ptIdx) ? e.v : e.u;
                            if (!pointQueued[otherPt])
                            {
                                pointQueue.push_back(otherPt);
                                pointQueued[otherPt] = true;
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

    int selectNextEdge(const State &s) const
    {
        int bestEdge = -1;
        int bestScore = -1000;

        // Look for critical edges
        for (int i = 0; i < (int)edges.size(); ++i)
        {
            if (s.edgeState[i] != 0)
                continue;

            const Edge &e = edges[i];
            int score = 0;

            // Critical priority: edges at points with degree 1
            int degU = s.pointDegree[e.u];
            int degV = s.pointDegree[e.v];
            if (degU == 1)
                score += 10000;
            if (degV == 1)
                score += 10000;

            // Very high priority: points with degree 0 and only 2 undecided
            int undU = s.pointUndecided[e.u];
            int undV = s.pointUndecided[e.v];
            if (degU == 0 && undU == 2)
                score += 5000;
            if (degV == 0 && undV == 2)
                score += 5000;

            // High priority: cells that need exactly their remaining edges
            if (e.cellA >= 0 && grid.clues[e.cellA] >= 0)
            {
                int clue = grid.clues[e.cellA];
                int onCount = s.cellEdgeCount[e.cellA];
                int undecided = s.cellUndecided[e.cellA];

                if (undecided > 0)
                {
                    int needed = clue - onCount;
                    if (needed == undecided)
                        score += 2000;
                    else if (needed == 0)
                        score += 2000;
                    else if (undecided == 1)
                        score += 1500;
                    else if (undecided <= 2)
                        score += 1000;
                    else
                        score += max(0, 100 - abs(needed * 2 - undecided));
                }
            }
            if (e.cellB >= 0 && grid.clues[e.cellB] >= 0)
            {
                int clue = grid.clues[e.cellB];
                int onCount = s.cellEdgeCount[e.cellB];
                int undecided = s.cellUndecided[e.cellB];

                if (undecided > 0)
                {
                    int needed = clue - onCount;
                    if (needed == undecided)
                        score += 2000;
                    else if (needed == 0)
                        score += 2000;
                    else if (undecided == 1)
                        score += 1500;
                    else if (undecided <= 2)
                        score += 1000;
                    else
                        score += max(0, 100 - abs(needed * 2 - undecided));
                }
            }

            if (score > bestScore)
            {
                bestScore = score;
                bestEdge = i;
            }

            // Early exit for critical edges
            if (bestScore >= 10000)
                return bestEdge;
        }

        return bestEdge >= 0 ? bestEdge : (int)edges.size();
    }

    bool finalCheckAndStore(State &s)
    {
        // Zellen-Bedingungen exakt prüfen
        for (size_t i = 0; i < grid.clues.size(); ++i)
        {
            int clue = grid.clues[i];
            if (clue >= 0 && s.cellEdgeCount[i] != clue)
                return false;
        }

#ifdef USE_TBB
        vector<int> onPrefix(edges.size());
        int onEdges = tbb::parallel_scan(
            tbb::blocked_range<size_t>(0, edges.size()),
            0,
            [&](const tbb::blocked_range<size_t> &r, int sum, bool isFinal)
            {
                for (size_t i = r.begin(); i != r.end(); ++i)
                {
                    int val = (s.edgeState[i] == 1) ? 1 : 0;
                    sum += val;
                    if (isFinal)
                        onPrefix[i] = sum;
                }
                return sum;
            },
            std::plus<int>());
#else
        vector<int> onPrefix(edges.size(), 0);
        int onEdges = 0;
        for (size_t i = 0; i < edges.size(); ++i)
        {
            onEdges += (s.edgeState[i] == 1) ? 1 : 0;
            onPrefix[i] = onEdges;
        }
#endif
        if (onEdges == 0)
            return false;

        vector<int> onEdgeIndices(onEdges);
#ifdef USE_TBB
        vector<size_t> edgeIndices(edges.size());
        iota(edgeIndices.begin(), edgeIndices.end(), 0);
        tbb::parallel_for_each(edgeIndices.begin(), edgeIndices.end(),
                               [&](size_t idx)
                               {
                                   if (s.edgeState[idx] == 1)
                                   {
                                       int pos = onPrefix[idx] - 1;
                                       onEdgeIndices[pos] = (int)idx;
                                   }
                               });
#else
        for (size_t i = 0; i < edges.size(); ++i)
        {
            if (s.edgeState[i] == 1)
            {
                int pos = onPrefix[i] - 1;
                onEdgeIndices[pos] = (int)i;
            }
        }
#endif

        // Adjazenzliste für alle eingeschalteten Kanten
        vector<vector<int>> adj(numPoints);
        for (int v = 0; v < numPoints; ++v)
            adj[v].reserve(s.pointDegree[v]);
        for (int idx : onEdgeIndices)
        {
            const Edge &e = edges[idx];
            adj[e.u].push_back(e.v);
            adj[e.v].push_back(e.u);
        }

        // Jeder Punkt mit Grad > 0 muss Grad 2 haben
        int start = -1;
        for (int v = 0; v < numPoints; ++v)
        {
            int deg = s.pointDegree[v];
            if (deg != 0 && deg != 2)
                return false;
            if (deg == 2 && start == -1)
                start = v;
        }
        if (start == -1)
            return false;

        // DFS/BFS: eine zusammenhängende Komponente?
        vector<char> vis(numPoints, 0);
        int visitedVertices = 0;
        int visitedEdges = 0;
        {
            stack<int> st;
            st.push(start);
            vis[start] = 1;
            while (!st.empty())
            {
                int v = st.top();
                st.pop();
                visitedVertices++;
                for (int to : adj[v])
                {
                    visitedEdges++; // Jede Kante wird insgesamt doppelt gezählt
                    if (!vis[to])
                    {
                        vis[to] = 1;
                        st.push(to);
                    }
                }
            }
        }
        int componentEdges = visitedEdges / 2;

        // Alle Punkte mit Grad 2 müssen in der Komponente sein
        for (int v = 0; v < numPoints; ++v)
        {
            if (s.pointDegree[v] == 2 && !vis[v])
                return false;
        }

        // Komponente muss alle eingeschalteten Kanten enthalten
        if (componentEdges != onEdges)
            return false;

        // Geschlossenen Zyklus in Reihenfolge aufbauen
        vector<pair<int, int>> cycle;
        {
            int n = grid.n, m = grid.m;
            auto coord = [m](int id)
            {
                int cols = m + 1;
                return make_pair(id / cols, id % cols);
            };
            int cur = start;
            int prev = -1;
            while (true)
            {
                cycle.push_back(coord(cur));
                int next = -1;
                for (int to : adj[cur])
                {
                    if (to != prev)
                    {
                        next = to;
                        break;
                    }
                }
                if (next == -1)
                    break;
                prev = cur;
                cur = next;
                if (cur == start)
                {
                    cycle.push_back(coord(cur));
                    break;
                }
            }
        }

        Solution sol;
        sol.edgeState = s.edgeState;
        sol.cyclePoints = cycle;

#ifdef USE_TBB
        int solNum = ++solutionCount;
        cout << "\n=== Solution " << solNum << " found! ===\n";
        printSolution(sol);
        cout << flush;

        tbbSolutions.push_back(sol);
        if (!findAll)
            stopAfterFirst.store(true, memory_order_relaxed);
#else
        {
            lock_guard<mutex> lock(solMutex);
            int solNum = ++solutionCount;

            // Print solution immediately
            cout << "\n=== Solution " << solNum << " found! ===\n";
            Solution tempSol = sol;
            printSolution(tempSol);
            cout << flush;

            solutions.push_back(std::move(sol));
            if (!findAll)
            {
                stopAfterFirst.store(true, memory_order_relaxed);
            }
        }
#endif
        return true;
    }

    void search(State s, int depth)
    {
        if (!findAll && stopAfterFirst.load(memory_order_relaxed))
            return;

        if (!quickValidityCheck(s))
            return;

        if (!propagateConstraints(s))
            return;

        int edgeIdx = selectNextEdge(s);
        if (edgeIdx == (int)edges.size())
        {
            finalCheckAndStore(s);
            return;
        }

        const Edge &edge = edges[edgeIdx];
        bool canOff = true;
        bool canOn = true;

        int degU = s.pointDegree[edge.u];
        int degV = s.pointDegree[edge.v];
        int undU = s.pointUndecided[edge.u];
        int undV = s.pointUndecided[edge.v];

        if ((degU == 1 && undU == 1) || (degV == 1 && undV == 1))
            canOff = false;
        if (degU >= 2 || degV >= 2)
            canOn = false;

        State offState;
        if (canOff)
        {
            offState = s;
            if (!(applyDecision(offState, edgeIdx, -1) && quickValidityCheck(offState) && propagateConstraints(offState)))
                canOff = false;
        }

        State onState;
        if (canOn)
        {
            onState = s;
            if (!(applyDecision(onState, edgeIdx, 1) && quickValidityCheck(onState) && propagateConstraints(onState)))
                canOn = false;
        }

        if (!canOn && !canOff)
            return;
        if (canOn && !canOff)
        {
            search(std::move(onState), depth + 1);
            return;
        }
        if (!canOn && canOff)
        {
            search(std::move(offState), depth + 1);
            return;
        }

#ifdef USE_TBB
        if (depth < maxParallelDepth)
        {
            tbb::task_group g;
            g.run([this, off = std::move(offState), depth]()
                  {
                      State local = off;
                      search(std::move(local), depth + 1);
                  });
            search(std::move(onState), depth + 1);
            g.wait();
        }
        else
        {
            search(std::move(offState), depth + 1);
            if (!findAll && stopAfterFirst.load(memory_order_relaxed))
                return;
            search(std::move(onState), depth + 1);
        }
#else
        bool doParallel = (depth < maxParallelDepth &&
                           activeThreads.load(memory_order_relaxed) < maxThreads);
        if (doParallel)
        {
            activeThreads.fetch_add(1, memory_order_relaxed);
            auto fut = std::async(std::launch::async, [this, off = std::move(offState), depth]() mutable
                                  {
                                      search(std::move(off), depth + 1);
                                      activeThreads.fetch_sub(1, memory_order_relaxed);
                                  });
            search(std::move(onState), depth + 1);
            fut.get();
        }
        else
        {
            search(std::move(offState), depth + 1);
            if (!findAll && stopAfterFirst.load(memory_order_relaxed))
                return;
            search(std::move(onState), depth + 1);
        }
#endif
    }

    void run(bool allSolutions)
    {
        findAll = allSolutions;
        stopAfterFirst.store(false, memory_order_relaxed);
        solutionCount.store(0, memory_order_relaxed);

#ifdef USE_TBB
        int numThreads = (int)thread::hardware_concurrency();
        cout << "Using Intel oneAPI TBB with " << numThreads << " threads\n";
        arena = make_unique<tbb::task_arena>(numThreads);
        tbbSolutions.clear();
#endif

        cout << "Searching for " << (allSolutions ? "all solutions" : "first solution") << "...\n"
             << flush;

        buildEdges();
        State startState = initialState();

#ifdef USE_TBB
        arena->execute([this, startState]()
                       { search(startState, 0); });

        solutions.clear();
        for (const auto &sol : tbbSolutions)
            solutions.push_back(sol);
#else
        search(std::move(startState), 0);
#endif
    }

    void printSolution(const Solution &sol) const
    {
        int n = grid.n, m = grid.m;
        auto isHorizOn = [&](int r, int c) -> bool
        {
            int idx = horizEdgeIndex[r * m + c];
            return sol.edgeState[idx] == 1;
        };
        auto isVertOn = [&](int r, int c) -> bool
        {
            int idx = vertEdgeIndex[r * (m + 1) + c];
            return sol.edgeState[idx] == 1;
        };

        // Zeilen mit Punkten und Horizontal-Kanten
        for (int r = 0; r <= n; ++r)
        {
            string line;
            for (int c = 0; c < m; ++c)
            {
                line += "+";
                line += (isHorizOn(r, c) ? "-" : " ");
            }
            line += "+";
            cout << line << "\n";

            if (r == n)
                break;

            // Zeilen mit Vertikal-Kanten und Ziffern
            string vline;
            for (int c = 0; c < m; ++c)
            {
                vline += (isVertOn(r, c) ? "|" : " ");
                int clue = grid.clues[grid.cellIndex(r, c)];
                char ch = ' ';
                if (clue >= 0)
                    ch = char('0' + clue);
                vline += ch;
            }
            // rechte Randkante
            vline += (isVertOn(r, m) ? "|" : " ");
            cout << vline << "\n";
        }

        // Zyklus als Liste von Punktkoordinaten
        cout << "Cycle (point coordinates row,col):\n";
        for (size_t i = 0; i < sol.cyclePoints.size(); ++i)
        {
            auto [r, c] = sol.cyclePoints[i];
            cout << "(" << r << "," << c << ")";
            if (i + 1 < sol.cyclePoints.size())
                cout << " -> ";
        }
        cout << "\n";
    }

    void printSolutions() const
    {
        if (solutions.empty())
        {
            cout << "\nNo solutions found.\n";
            return;
        }
        cout << "\n=== SUMMARY ===\n";
        cout << "Total solutions found: " << solutions.size() << "\n";
    }
};

Grid readGridFromFile(const string &filename)
{
    ifstream in(filename);
    if (!in)
    {
        throw runtime_error("Could not open file " + filename);
    }
    Grid g;
    in >> g.n >> g.m;
    string line;
    getline(in, line); // Rest der ersten Zeile wegwerfen

    g.clues.assign(g.n * g.m, -1);

    for (int r = 0; r < g.n; ++r)
    {
        if (!getline(in, line))
        {
            throw runtime_error("Not enough grid lines in file");
        }
        if (line.empty())
        {
            r--;
            continue;
        } // leere Zeilen überspringen
        vector<int> row;
        for (char ch : line)
        {
            if (ch == ' ' || ch == '\t')
                continue;
            if (ch >= '0' && ch <= '3')
            {
                row.push_back(ch - '0');
            }
            else
            {
                row.push_back(-1);
            }
            if ((int)row.size() == g.m)
                break;
        }
        if ((int)row.size() != g.m)
        {
            throw runtime_error("Row " + to_string(r) + " does not have m entries");
        }
        for (int c = 0; c < g.m; ++c)
        {
            g.clues[g.cellIndex(r, c)] = row[c];
        }
    }
    return g;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <inputfile> [--all]\n";
        return 1;
    }
    string filename = argv[1];
    bool allSolutions = false;
    if (argc >= 3)
    {
        string arg2 = argv[2];
        if (arg2 == "--all")
            allSolutions = true;
    }

    try
    {
        Grid g = readGridFromFile(filename);
        Solver solver;
        solver.grid = std::move(g);

        auto start = chrono::steady_clock::now();
        solver.run(allSolutions);
        auto end = chrono::steady_clock::now();
        double seconds = chrono::duration_cast<chrono::duration<double>>(end - start).count();

        solver.printSolutions();
        cout << "Time: " << seconds << " s\n";
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
