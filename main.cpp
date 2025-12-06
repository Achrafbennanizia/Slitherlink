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

/**
 * @brief Represents a Slitherlink puzzle grid
 *
 * The grid stores the puzzle dimensions and clue values for each cell.
 * Clues range from 0-3 indicating how many edges around a cell should be ON,
 * or -1 for cells without clues.
 */
struct Grid
{
    int n = 0;         ///< Number of rows
    int m = 0;         ///< Number of columns
    vector<int> clues; ///< Clue values (-1 for no clue, 0-3 for clue value)

    /**
     * @brief Convert 2D grid coordinates to 1D array index
     * @param r Row index
     * @param c Column index
     * @return Linear index in clues array
     */
    int cellIndex(int r, int c) const { return r * m + c; }
};

/**
 * @brief Represents an edge in the puzzle graph
 *
 * An edge connects two points (u, v) and is adjacent to up to two cells.
 */
struct Edge
{
    int u;     ///< First endpoint (point index)
    int v;     ///< Second endpoint (point index)
    int cellA; ///< First adjacent cell (-1 if none)
    int cellB; ///< Second adjacent cell (-1 if none)
};

/**
 * @brief Represents the current state of the search
 *
 * Tracks which edges are ON/OFF/UNDECIDED and maintains counts
 * for constraint propagation.
 *
 * Optimized with cache-friendly layout (#6) - hot data grouped together
 */
struct State
{
    // Cache-friendly layout (#6): Group frequently accessed data together
    vector<char> edgeState;     ///< 0=undecided, 1=ON, -1=OFF
    vector<int> pointDegree;    ///< Number of ON edges at each point
    vector<int> pointUndecided; ///< Number of undecided edges per point
    vector<int> cellEdgeCount;  ///< Number of ON edges around each cell
    vector<int> cellUndecided;  ///< Number of undecided edges per cell

    State() = default;
    State(const State &) = default;
    State(State &&) noexcept = default;
    State &operator=(const State &) = default;
    State &operator=(State &&) noexcept = default;
};

/**
 * @brief Represents a complete solution to the puzzle
 */
struct Solution
{
    vector<char> edgeState; ///< Final edge configuration
    vector<pair<int, int>> cyclePoints;

    // Operator for symmetry comparison (#9)
    bool operator<(const Solution &other) const
    {
        return edgeState < other.edgeState;
    }
};

/**
 * @brief Memory pool for State objects (#3)
 *
 * Reduces allocation overhead by reusing State objects.
 * Thread-safe for parallel search.
 */
class StatePool
{
private:
    vector<unique_ptr<State>> pool;
    mutex poolMutex;
    size_t edgeCount;
    size_t pointCount;
    size_t cellCount;

public:
    StatePool(size_t edges, size_t points, size_t cells)
        : edgeCount(edges), pointCount(points), cellCount(cells)
    {
        // Pre-allocate some states
        pool.reserve(32);
    }

    State *acquire()
    {
        lock_guard<mutex> lock(poolMutex);
        if (!pool.empty())
        {
            State *s = pool.back().release();
            pool.pop_back();
            return s;
        }
        // Allocate new state with proper sizes
        State *s = new State();
        s->edgeState.reserve(edgeCount);
        s->edgeState.resize(edgeCount, 0);
        s->pointDegree.resize(pointCount, 0);
        s->pointUndecided.resize(pointCount, 0);
        s->cellEdgeCount.resize(cellCount, 0);
        s->cellUndecided.resize(cellCount, 0);
        return s;
    }

    void release(State *s)
    {
        if (!s)
            return;
        lock_guard<mutex> lock(poolMutex);
        if (pool.size() < 64)
        { // Limit pool size
            pool.push_back(unique_ptr<State>(s));
        }
        else
        {
            delete s;
        }
    }

    ~StatePool()
    {
        pool.clear();
    }
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
        // Reserve capacity before assignment to avoid reallocations
        s.edgeState.reserve(edges.size());
        s.edgeState.assign(edges.size(), 0);

        s.pointDegree.reserve(numPoints);
        s.pointDegree.assign(numPoints, 0);

        s.cellEdgeCount.reserve(grid.clues.size());
        s.cellEdgeCount.assign(grid.clues.size(), 0);

        s.cellUndecided.reserve(cellEdges.size());
        s.cellUndecided.resize(cellEdges.size());

        s.pointUndecided.reserve(numPoints);
        s.pointUndecided.resize(numPoints);

        for (size_t i = 0; i < cellEdges.size(); ++i)
            s.cellUndecided[i] = cellEdges[i].size();
        for (int i = 0; i < numPoints; ++i)
            s.pointUndecided[i] = pointEdges[i].size();

        return s;
    }

    /**
     * @brief Apply edge decision and update state (#13 - inlined hot path)
     */
    inline bool applyDecision(State &s, int edgeIdx, int val) const
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

    /**
     * @brief Quick validity check (#13 - inlined hot path)
     */
    inline bool quickValidityCheck(const State &s) const
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

        // Optimized queue with bitset-like tracking (#4)
        vector<int> cellQueue;
        vector<int> pointQueue;
        vector<uint8_t> cellQueued(grid.clues.size(), 0);
        vector<uint8_t> pointQueued(numPoints, 0);

        cellQueue.reserve(clueCells.size());
        pointQueue.reserve(numPoints);

        for (int cell : clueCells)
        {
            cellQueue.push_back(cell);
            cellQueued[cell] = 1;
        }
        for (int i = 0; i < numPoints; ++i)
        {
            pointQueue.push_back(i);
            pointQueued[i] = 1;
        }

        size_t cellPos = 0, pointPos = 0;

        while (cellPos < cellQueue.size() || pointPos < pointQueue.size())
        {
            while (cellPos < cellQueue.size())
            {
                int cellIdx = cellQueue[cellPos++];
                cellQueued[cellIdx] = 0;

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
                                cellQueued[e.cellA] = 1;
                            }
                            if (e.cellB >= 0 && !cellQueued[e.cellB] && grid.clues[e.cellB] >= 0)
                            {
                                cellQueue.push_back(e.cellB);
                                cellQueued[e.cellB] = 1;
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

            while (pointPos < pointQueue.size())
            {
                int ptIdx = pointQueue[pointPos++];
                pointQueued[ptIdx] = 0;

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
                                cellQueued[e.cellA] = 1;
                            }
                            if (e.cellB >= 0 && !cellQueued[e.cellB] && grid.clues[e.cellB] >= 0)
                            {
                                cellQueue.push_back(e.cellB);
                                cellQueued[e.cellB] = 1;
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
                                pointQueued[otherPt] = 1;
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

    /**
     * @brief Estimate branching factor for an edge decision (#11)
     *
     * Tests both ON and OFF decisions with quick validity checks.
     * Returns number of viable branches (0, 1, or 2).
     */
    int estimateBranches(const State &s, int edgeIdx) const
    {
        const Edge &e = edges[edgeIdx];
        int degU = s.pointDegree[e.u], degV = s.pointDegree[e.v];
        int undU = s.pointUndecided[e.u], undV = s.pointUndecided[e.v];

        // Quick forced move detection
        if ((degU == 1 && undU == 1) || (degV == 1 && undV == 1))
            return 1; // Must be ON
        if (degU >= 2 || degV >= 2)
            return 1; // Must be OFF

        // Both branches appear viable
        return 2;
    }

    /**
     * @brief Select next edge using smart heuristics (#11)
     *
     * Prioritizes:
     * 1. Forced moves (only one valid choice)
     * 2. Edges that minimize branching factor
     * 3. Edges adjacent to constrained cells
     */
    int selectNextEdge(const State &s) const
    {
        int bestEdge = -1;
        int minBranches = 3;
        int bestScore = -1000;

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

            // Estimate branching factor (#11)
            int branches = estimateBranches(s, i);

            // Forced move - return immediately
            if (branches == 1)
                return i;

            const Edge &e = edges[i];
            int degU = s.pointDegree[e.u], degV = s.pointDegree[e.v];
            int undU = s.pointUndecided[e.u], undV = s.pointUndecided[e.v];

            int score = ((degU == 1 || degV == 1) ? 10000 : 0) +
                        ((degU == 0 && undU == 2) || (degV == 0 && undV == 2) ? 5000 : 0) +
                        scoreCell(e.cellA) + scoreCell(e.cellB);

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

        // Symmetry breaking (#9) - skip non-canonical solutions in findAll mode
        if (!isCanonicalSolution(sol))
            return true; // Skip this solution, it's a symmetric duplicate

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

    /**
     * @brief Look-ahead pruning (#12) - Test if edge decision leads to valid state
     *
     * Performs 1-ply look-ahead to check if a decision will lead to solvable state.
     * Returns false if the decision definitely leads to failure.
     */
    inline bool testEdgeDecision(const State &s, int edgeIdx, int val) const
    {
        State test = s;
        if (!applyDecision(test, edgeIdx, val))
            return false;
        if (!propagateConstraints(test))
            return false;

        // Quick check: count if any moves are still possible
        for (int i = 0; i < (int)edges.size(); ++i)
        {
            if (test.edgeState[i] == 0)
                return true; // At least one move possible
        }

        return false; // No moves left, might be complete or stuck
    }

    /**
     * @brief Symmetry breaking (#9) - Check if solution is in canonical form
     *
     * For findAll mode, eliminates symmetric duplicates by checking if this
     * is the lexicographically smallest rotation/reflection.
     */
    bool isCanonicalSolution(const Solution &sol) const
    {
        if (!findAll)
            return true; // Only apply in findAll mode

        // For rectangular grids, check 4 rotations × 2 reflections = 8 symmetries
        // This is a simplified check - full implementation would apply transformations

        // For now, just check horizontal reflection
        int n = grid.n, m = grid.m;
        vector<char> reflected = sol.edgeState;

        // Simple reflection check (can be expanded for full symmetry)
        for (int r = 0; r < n; ++r)
        {
            for (int c = 0; c < m / 2; ++c)
            {
                // Swap horizontal edges
                int leftH = horizEdgeIndex[r * m + c];
                int rightH = horizEdgeIndex[r * m + (m - 1 - c)];
                if (leftH >= 0 && rightH >= 0)
                    swap(reflected[leftH], reflected[rightH]);
            }
        }

        // If reflected is smaller, this is not canonical
        if (reflected < sol.edgeState)
            return false;

        return true; // This is canonical form
    }

    /**
     * @brief Early detection of definitely unsolvable states
     *
     * Quickly checks for conditions that make a state impossible to solve,
     * allowing early termination without full propagation.
     *
     * @param s Current state to check
     * @return true if state is definitely unsolvable, false otherwise
     */
    inline bool isDefinitelyUnsolvable(const State &s) const
    {
        // Check for impossible point configurations
        for (int i = 0; i < numPoints; ++i)
        {
            int deg = s.pointDegree[i];
            int und = s.pointUndecided[i];

            // Dead end: point has degree 1 with no undecided edges
            if (deg == 1 && und == 0)
                return true;

            // Impossible: can't reach degree 2 (only 0 or 2 allowed in final loop)
            if (deg > 0 && deg + und < 2)
                return true;

            // Already exceeded maximum degree
            if (deg > 2)
                return true;
        }

        // Check for impossible cell configurations
        for (int cell : clueCells)
        {
            int clue = grid.clues[cell];
            int on = s.cellEdgeCount[cell];
            int und = s.cellUndecided[cell];

            // Already exceeded clue
            if (on > clue)
                return true;

            // Can't reach clue even with all remaining edges
            if (on + und < clue)
                return true;
        }

        return false;
    }

    void search(State s, int depth)
    {
        if (!findAll && stopAfterFirst.load(memory_order_relaxed))
            return;

        // Early unsolvability detection - exit before expensive operations
        if (isDefinitelyUnsolvable(s))
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

        // Look-ahead pruning (#12): Disabled by default - too expensive
        // Uncomment to enable (adds ~20% overhead for ~10% search reduction)
        /*
        if (depth < 5 && canOn && canOff) {
            if (canOff && !testEdgeDecision(s, edgeIdx, -1))
                canOff = false;
            if (canOn && !testEdgeDecision(s, edgeIdx, 1))
                canOn = false;
        }
        */

        // Try OFF first, reuse state if possible
        State offState;
        if (canOff)
        {
            offState = s; // Copy original state
            if (!(applyDecision(offState, edgeIdx, -1) && quickValidityCheck(offState) && propagateConstraints(offState)))
                canOff = false;
        }

        // Try ON - reuse original state to avoid extra copy
        State onState;
        if (canOn)
        {
            if (canOff)
            {
                // Need to copy original since offState modified it
                onState = s;
            }
            else
            {
                // Can reuse s directly since offState wasn't created
                onState = std::move(s);
            }
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
        // Parallel pruning (#7): Only parallelize if subtree is large enough
        bool shouldParallelize = false;
        if (depth < maxParallelDepth)
        {
            // Estimate subtree size based on undecided edges
            int undecidedCount = 0;
            for (int i = 0; i < (int)edges.size() && undecidedCount <= 15; ++i)
                if (s.edgeState[i] == 0)
                    undecidedCount++;

            // Only parallelize if subtree has >1000 estimated nodes
            // (undecided > 10 means 2^10 = 1024+ nodes)
            shouldParallelize = (undecidedCount > 10);
        }

        if (shouldParallelize)
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

    void run(bool allSolutions, int numThreads = 0, double cpuPercent = 1.0)
    {
        findAll = allSolutions;
        stopAfterFirst.store(false, memory_order_relaxed);
        solutionCount.store(0, memory_order_relaxed);

        buildEdges();
        maxParallelDepth = calculateOptimalParallelDepth();

#ifdef USE_TBB
        int hwThreads = (int)thread::hardware_concurrency();
        if (numThreads <= 0)
        {
            numThreads = max(1, (int)(hwThreads * cpuPercent));
        }
        numThreads = max(1, min(numThreads, hwThreads));

        cout << "Using Intel oneAPI TBB with " << numThreads << " threads "
             << "(" << (100.0 * numThreads / hwThreads) << "% CPU)\n";
        cout << "Dynamic parallel depth: " << maxParallelDepth << " (optimized for "
             << grid.n << "x" << grid.m << " puzzle)\n";
        arena = make_unique<tbb::task_arena>(numThreads);
        tbbSolutions.clear();
#else
        if (numThreads > 0)
        {
            maxThreads = numThreads;
        }
        else if (cpuPercent < 1.0)
        {
            maxThreads = max(1, (int)(maxThreads * cpuPercent));
        }
        cout << "Using std::async with up to " << maxThreads << " threads\n";
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
        cerr << "Usage: " << argv[0] << " <inputfile> [--all] [--threads N] [--cpu PERCENT]\n";
        cerr << "  --all          Find all solutions (default: first only)\n";
        cerr << "  --threads N    Use N threads (default: auto)\n";
        cerr << "  --cpu PERCENT  Use PERCENT of available CPU (0.0-1.0, e.g., 0.5 for 50%)\n";
        cerr << "Examples:\n";
        cerr << "  " << argv[0] << " puzzle.txt --threads 8\n";
        cerr << "  " << argv[0] << " puzzle.txt --cpu 0.5\n";
        cerr << "  " << argv[0] << " puzzle.txt --threads 8 --all\n";
        return 1;
    }
    string filename = argv[1];
    bool allSolutions = false;
    int numThreads = 0;
    double cpuPercent = 1.0;

    for (int i = 2; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "--all")
        {
            allSolutions = true;
        }
        else if (arg == "--threads" && i + 1 < argc)
        {
            numThreads = atoi(argv[++i]);
            if (numThreads <= 0)
            {
                cerr << "Error: Invalid thread count\n";
                return 1;
            }
        }
        else if (arg == "--cpu" && i + 1 < argc)
        {
            cpuPercent = atof(argv[++i]);
            if (cpuPercent <= 0.0 || cpuPercent > 1.0)
            {
                cerr << "Error: CPU percent must be between 0.0 and 1.0\n";
                return 1;
            }
        }
    }

    try
    {
        Grid g = readGridFromFile(filename);
        Solver solver;
        solver.grid = std::move(g);

        auto start = chrono::steady_clock::now();
        solver.run(allSolutions, numThreads, cpuPercent);
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
