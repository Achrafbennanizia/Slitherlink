#include <algorithm>
#include <atomic>
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <stack>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#ifdef USE_TBB
#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/global_control.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <tbb/spin_mutex.h>
#include <tbb/task_arena.h>
#include <tbb/task_group.h>
#endif

using namespace std;

struct Grid
{
    int n = 0, m = 0;
    vector<int> clues;

    int cellIndex(int r, int c) const { return r * m + c; }
};

struct Edge
{
    int u, v;
    int cellA;
    int cellB;
};

struct State
{
    vector<char> edgeState;
    vector<int> pointDegree;
    vector<int> cellEdgeCount;
    vector<int> cellUndecided;
    vector<int> pointUndecided;

    State() = default;
    State(const State &) = default;
    State(State &&) noexcept = default;
    State &operator=(const State &) = default;
    State &operator=(State &&) noexcept = default;
};

struct Solution
{
    vector<char> edgeState;
    vector<pair<int, int>> cyclePoints;
};

struct Solver
{
    Grid grid;
    vector<Edge> edges;
    int numPoints = 0;

    vector<int> horizEdgeIndex;
    vector<int> vertEdgeIndex;
    vector<vector<int>> cellEdges;
    vector<vector<int>> pointEdges;
    vector<int> clueCells;

    bool findAll = false;
    atomic<bool> stopAfterFirst{false};

    mutex solMutex;
    vector<Solution> solutions;
    atomic<int> solutionCount{0};

    int maxParallelDepth = 16;
    atomic<int> activeThreads{0};
    int maxThreads = thread::hardware_concurrency() > 0 ? thread::hardware_concurrency() : 4;

#ifdef USE_TBB
    unique_ptr<tbb::task_arena> arena;
    tbb::concurrent_vector<Solution> tbbSolutions;
#endif

    int calculateOptimalParallelDepth()
    {
        int totalCells = grid.n * grid.m;
        int clueCount = count_if(grid.clues.begin(), grid.clues.end(), [](int c)
                                 { return c >= 0; });
        double density = (double)clueCount / totalCells;

        int depth = (totalCells <= 25) ? 8 : (totalCells <= 49) ? 12
                                         : (totalCells <= 64)   ? 14
                                         : (totalCells <= 100)  ? 32
                                         : (totalCells <= 144)  ? 34
                                         : (totalCells <= 225)  ? 36
                                                                : 38;

        if (density < 0.3)
            depth += 6; // Sparse puzzles need more parallelism
        return max(10, min(45, depth));
    }

    void buildEdges()
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

        // Build horizontal edges
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

        // Build vertical edges
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

    State initialState() const
    {
        State s;
        s.edgeState.assign(edges.size(), 0);
        s.pointDegree.assign(numPoints, 0);
        s.cellEdgeCount.assign(grid.clues.size(), 0);
        s.cellUndecided.resize(cellEdges.size());
        s.pointUndecided.resize(numPoints);

        for (size_t i = 0; i < cellEdges.size(); ++i)
            s.cellUndecided[i] = cellEdges[i].size();
        for (int i = 0; i < numPoints; ++i)
            s.pointUndecided[i] = pointEdges[i].size();

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
            tbb::blocked_range<int>(0, numPoints), true,
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
            tbb::blocked_range<size_t>(0, clueCells.size()), true,
            [&](const tbb::blocked_range<size_t> &r, bool ok) -> bool
            {
                if (!ok)
                    return false;
                for (size_t i = r.begin(); i != r.end(); ++i)
                {
                    int cell = clueCells[i];
                    int clue = grid.clues[cell];
                    if (s.cellEdgeCount[cell] > clue)
                        return false;
                    if (s.cellEdgeCount[cell] + s.cellUndecided[cell] < clue)
                        return false;
                }
                return true;
            },
            [](bool a, bool b)
            { return a && b; });
        return cellsOk;
#else
        for (int i = 0; i < numPoints; ++i)
        {
            if (s.pointDegree[i] > 2)
                return false;
            if (s.pointDegree[i] == 1 && s.pointUndecided[i] == 0)
                return false;
        }

        for (int cell : clueCells)
        {
            int clue = grid.clues[cell];
            if (s.cellEdgeCount[cell] > clue)
                return false;
            if (s.cellEdgeCount[cell] + s.cellUndecided[cell] < clue)
                return false;
        }
        return true;
#endif
    }

    bool propagateConstraints(State &s) const
    {
        for (int cell : clueCells)
        {
            int clue = grid.clues[cell];
            int onCount = s.cellEdgeCount[cell];
            int undecided = s.cellUndecided[cell];
            int maxPossible = onCount + undecided;

            if (onCount > clue || maxPossible < clue)
                return false;
        }

        vector<int> cellQueue;
        vector<int> pointQueue;
        vector<bool> cellQueued(grid.clues.size(), false);
        vector<bool> pointQueued(numPoints, false);

        cellQueue.reserve(clueCells.size());
        pointQueue.reserve(numPoints);

        for (int cell : clueCells)
        {
            cellQueue.push_back(cell);
            cellQueued[cell] = true;
        }
        for (int i = 0; i < numPoints; ++i)
        {
            pointQueue.push_back(i);
            pointQueued[i] = true;
        }

        size_t cellPos = 0, pointPos = 0;

        while (cellPos < cellQueue.size() || pointPos < pointQueue.size())
        {
            while (cellPos < cellQueue.size())
            {
                int cellIdx = cellQueue[cellPos++];
                cellQueued[cellIdx] = false;

                int clue = grid.clues[cellIdx];
                if (clue < 0)
                    continue;

                int onCount = s.cellEdgeCount[cellIdx];
                int undecided = s.cellUndecided[cellIdx];

                if (onCount + undecided == clue)
                {
                    for (int eidx : cellEdges[cellIdx])
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

            while (pointPos < pointQueue.size())
            {
                int ptIdx = pointQueue[pointPos++];
                pointQueued[ptIdx] = false;

                int deg = s.pointDegree[ptIdx];
                int undecided = s.pointUndecided[ptIdx];

                if (deg >= 2 || (deg == 0 && undecided == 0))
                    continue;

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
        int bestEdge = -1, bestScore = -1000;

        auto scoreCell = [&](int cellIdx) -> int
        {
            if (cellIdx < 0 || grid.clues[cellIdx] < 0)
                return 0;
            int clue = grid.clues[cellIdx], cnt = s.cellEdgeCount[cellIdx], und = s.cellUndecided[cellIdx];
            if (und == 0)
                return 0;
            int need = clue - cnt;
            return (need == und || need == 0) ? 2000 : (und == 1) ? 1500
                                                   : (und <= 2)   ? 1000
                                                                  : max(0, 100 - abs(need * 2 - und));
        };

        for (int i = 0; i < (int)edges.size(); ++i)
        {
            if (s.edgeState[i] != 0)
                continue;
            const Edge &e = edges[i];
            int degU = s.pointDegree[e.u], degV = s.pointDegree[e.v];
            int undU = s.pointUndecided[e.u], undV = s.pointUndecided[e.v];

            int score = ((degU == 1 || degV == 1) ? 10000 : 0) +
                        ((degU == 0 && undU == 2) || (degV == 0 && undV == 2) ? 5000 : 0) +
                        scoreCell(e.cellA) + scoreCell(e.cellB);

            if (score > bestScore)
            {
                bestScore = score;
                bestEdge = i;
                if (bestScore >= 10000)
                    return bestEdge;
            }
        }
        return bestEdge >= 0 ? bestEdge : (int)edges.size();
    }

    bool finalCheckAndStore(State &s)
    {
#ifdef USE_TBB
        bool valid = tbb::parallel_reduce(
            tbb::blocked_range<size_t>(0, clueCells.size()), true,
            [&](const tbb::blocked_range<size_t> &r, bool v)
            {
                for (size_t i = r.begin(); i < r.end() && v; ++i)
                    if (s.cellEdgeCount[clueCells[i]] != grid.clues[clueCells[i]])
                        v = false;
                return v;
            },
            [](bool a, bool b)
            { return a && b; });
        if (!valid)
            return false;
#else
        for (int cell : clueCells)
            if (s.cellEdgeCount[cell] != grid.clues[cell])
                return false;
#endif

        vector<vector<int>> adj(numPoints);
        int start = -1;

#ifdef USE_TBB
        tbb::parallel_for(tbb::blocked_range<int>(0, numPoints),
                          [&](const tbb::blocked_range<int> &r)
                          {
                              for (int v = r.begin(); v < r.end(); ++v)
                                  adj[v].reserve(s.pointDegree[v]);
                          });

        tbb::spin_mutex startMutex;
        tbb::parallel_for(tbb::blocked_range<size_t>(0, edges.size()),
                          [&](const tbb::blocked_range<size_t> &r)
                          {
                              for (size_t i = r.begin(); i < r.end(); ++i)
                              {
                                  if (s.edgeState[i] == 1)
                                  {
                                      const Edge &e = edges[i];
                                      adj[e.u].push_back(e.v);
                                      adj[e.v].push_back(e.u);
                                      if (start == -1)
                                      {
                                          tbb::spin_mutex::scoped_lock lock(startMutex);
                                          if (start == -1)
                                              start = e.u;
                                      }
                                  }
                              }
                          });
#else
        for (int v = 0; v < numPoints; ++v)
            adj[v].reserve(s.pointDegree[v]);
        for (size_t i = 0; i < edges.size(); ++i)
        {
            if (s.edgeState[i] == 1)
            {
                const Edge &e = edges[i];
                adj[e.u].push_back(e.v);
                adj[e.v].push_back(e.u);
                if (start == -1)
                    start = e.u;
            }
        }
#endif
        if (start == -1)
            return false;

        int onEdges = 0;
#ifdef USE_TBB
        auto result = tbb::parallel_reduce(
            tbb::blocked_range<int>(0, numPoints),
            make_pair(true, 0),
            [&](const tbb::blocked_range<int> &r, pair<bool, int> res)
            {
                for (int v = r.begin(); v < r.end() && res.first; ++v)
                {
                    int deg = adj[v].size();
                    if (deg != 0 && deg != 2)
                        res.first = false;
                    res.second += deg;
                }
                return res;
            },
            [](pair<bool, int> a, pair<bool, int> b)
            {
                return make_pair(a.first && b.first, a.second + b.second);
            });
        if (!result.first)
            return false;
        onEdges = result.second / 2;
#else
        for (int v = 0; v < numPoints; ++v)
        {
            int deg = adj[v].size();
            if (deg != 0 && deg != 2)
                return false;
            onEdges += deg;
        }
        onEdges /= 2;
#endif
        if (onEdges == 0)
            return false;

        vector<char> vis(numPoints, 0);
        int visitedEdges = 0;
        stack<int> st;
        st.push(start);
        vis[start] = 1;
        while (!st.empty())
        {
            int v = st.top();
            st.pop();
            for (int to : adj[v])
            {
                visitedEdges++;
                if (!vis[to])
                {
                    vis[to] = 1;
                    st.push(to);
                }
            }
        }

#ifdef USE_TBB
        bool allVisited = tbb::parallel_reduce(
            tbb::blocked_range<int>(0, numPoints), true,
            [&](const tbb::blocked_range<int> &r, bool v)
            {
                for (int i = r.begin(); i < r.end() && v; ++i)
                    if (adj[i].size() == 2 && !vis[i])
                        v = false;
                return v;
            },
            [](bool a, bool b)
            { return a && b; });
        if (!allVisited || visitedEdges / 2 != onEdges)
            return false;
#else
        for (int v = 0; v < numPoints; ++v)
            if (adj[v].size() == 2 && !vis[v])
                return false;
        if (visitedEdges / 2 != onEdges)
            return false;
#endif

        vector<pair<int, int>> cycle;
        int cols = grid.m + 1;
        auto coord = [cols](int id)
        { return make_pair(id / cols, id % cols); };

        int cur = start, prev = -1;
        do
        {
            cycle.push_back(coord(cur));
            int next = (adj[cur][0] != prev) ? adj[cur][0] : adj[cur][1];
            prev = cur;
            cur = next;
        } while (cur != start);
        cycle.push_back(coord(start));

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
                      search(std::move(local), depth + 1); });
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
                                      activeThreads.fetch_sub(1, memory_order_relaxed); });
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

        buildEdges();
        maxParallelDepth = calculateOptimalParallelDepth();

#ifdef USE_TBB
        int numThreads = max(1, (int)thread::hardware_concurrency());
        cout << "Using Intel oneAPI TBB with " << numThreads << " threads (100% CPU)\n";
        cout << "Dynamic parallel depth: " << maxParallelDepth << " (optimized for "
             << grid.n << "x" << grid.m << " puzzle)\n";
        arena = make_unique<tbb::task_arena>(numThreads);
        tbbSolutions.clear();
#endif

        cout << "Searching for " << (allSolutions ? "all solutions" : "first solution") << "...\n"
             << flush;

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
            vline += (isVertOn(r, m) ? "|" : " ");
            cout << vline << "\n";
        }

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
    getline(in, line);

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
        }
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
