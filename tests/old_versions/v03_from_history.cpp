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
    vector<char> edgeState;    // 0 undecided, 1 on, -1 off
    vector<int> pointDegree;   // degree of each point from ON edges
    vector<int> cellEdgeCount; // count ON edges around each cell

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
    atomic<int> activeThreads{0};

    mutex solMutex;
    vector<Solution> solutions;

    int maxParallelDepth = 6;
    int maxThreads = min(8, (int)thread::hardware_concurrency());

    void buildEdges()
    {
        int n = grid.n, m = grid.m;
        numPoints = (n + 1) * (m + 1);
        horizEdgeIndex.assign((n + 1) * m, -1);
        vertEdgeIndex.assign(n * (m + 1), -1);
        cellEdges.assign(n * m, vector<int>());
        pointEdges.assign(numPoints, vector<int>());

        edges.clear();
        int idx = 0;

        auto pointId = [this](int r, int c)
        {
            return r * (grid.m + 1) + c;
        };

        // horizontale Kanten
        for (int r = 0; r <= n; ++r)
        {
            for (int c = 0; c < m; ++c)
            {
                Edge e;
                e.u = pointId(r, c);
                e.v = pointId(r, c + 1);
                e.cellA = -1;
                e.cellB = -1;
                if (r > 0)
                {
                    e.cellA = grid.cellIndex(r - 1, c); // Zelle über der Kante
                }
                if (r < n)
                {
                    e.cellB = grid.cellIndex(r, c); // Zelle unter der Kante
                }
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
        }

        // vertikale Kanten
        for (int r = 0; r < n; ++r)
        {
            for (int c = 0; c <= m; ++c)
            {
                Edge e;
                e.u = pointId(r, c);
                e.v = pointId(r + 1, c);
                e.cellA = -1;
                e.cellB = -1;
                if (c > 0)
                {
                    e.cellA = grid.cellIndex(r, c - 1); // Zelle links der Kante
                }
                if (c < m)
                {
                    e.cellB = grid.cellIndex(r, c); // Zelle rechts der Kante
                }
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
        }
    }

    State initialState() const
    {
        State s;
        s.edgeState.assign(edges.size(), 0);
        s.pointDegree.assign(numPoints, 0);
        s.cellEdgeCount.assign(grid.clues.size(), 0);
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

    bool propagateConstraints(State &s) const
    {
        bool changed = true;
        while (changed)
        {
            changed = false;

            // Check each cell for forced edges
            for (size_t cellIdx = 0; cellIdx < grid.clues.size(); ++cellIdx)
            {
                int clue = grid.clues[cellIdx];
                if (clue < 0)
                    continue;

                int onCount = s.cellEdgeCount[cellIdx];
                int offCount = 0;
                int undecided = 0;

                for (int eidx : cellEdges[cellIdx])
                {
                    if (s.edgeState[eidx] == 1)
                        continue;
                    if (s.edgeState[eidx] == -1)
                        offCount++;
                    else
                        undecided++;
                }

                // If we need all remaining edges
                if (onCount + undecided == clue)
                {
                    for (int eidx : cellEdges[cellIdx])
                    {
                        if (s.edgeState[eidx] == 0)
                        {
                            if (!applyDecision(s, eidx, 1))
                                return false;
                            changed = true;
                        }
                    }
                }
                // If we have enough edges, turn off rest
                else if (onCount == clue)
                {
                    for (int eidx : cellEdges[cellIdx])
                    {
                        if (s.edgeState[eidx] == 0)
                        {
                            s.edgeState[eidx] = -1;
                            changed = true;
                        }
                    }
                }
            }

            // Check each point for forced edges
            for (int ptIdx = 0; ptIdx < numPoints; ++ptIdx)
            {
                int deg = s.pointDegree[ptIdx];
                if (deg >= 2)
                    continue;

                int undecided = 0;
                for (int eidx : pointEdges[ptIdx])
                {
                    if (s.edgeState[eidx] == 0)
                        undecided++;
                }

                if (deg == 0 && undecided == 0)
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
                            changed = true;
                        }
                    }
                }
                // If degree is 2, turn off remaining
                else if (deg == 2)
                {
                    for (int eidx : pointEdges[ptIdx])
                    {
                        if (s.edgeState[eidx] == 0)
                        {
                            s.edgeState[eidx] = -1;
                            changed = true;
                        }
                    }
                }
                // If degree is 0 and only 2 undecided, can't use just 1
                else if (deg == 0 && undecided == 2)
                {
                    // Must use 0 or 2, not 1
                }
            }
        }
        return true;
    }

    int selectNextEdge(const State &s, int startIdx) const
    {
        int bestEdge = -1;
        int bestScore = -1;

        for (int i = startIdx; i < (int)edges.size(); ++i)
        {
            if (s.edgeState[i] != 0)
                continue;

            const Edge &e = edges[i];
            int score = 0;

            // Prioritize edges adjacent to cells with clues
            if (e.cellA >= 0 && grid.clues[e.cellA] >= 0)
            {
                int clue = grid.clues[e.cellA];
                int onCount = s.cellEdgeCount[e.cellA];
                score += (10 - abs(clue - onCount - 1));
            }
            if (e.cellB >= 0 && grid.clues[e.cellB] >= 0)
            {
                int clue = grid.clues[e.cellB];
                int onCount = s.cellEdgeCount[e.cellB];
                score += (10 - abs(clue - onCount - 1));
            }

            // Prioritize edges at points with degree 1
            if (s.pointDegree[e.u] == 1 || s.pointDegree[e.v] == 1)
                score += 20;

            if (score > bestScore)
            {
                bestScore = score;
                bestEdge = i;
            }

            // Early exit for very constrained edges
            if (bestScore >= 30)
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

        // Adjazenzliste für alle eingeschalteten Kanten
        vector<vector<int>> adj(numPoints);
        int onEdges = 0;
        for (size_t i = 0; i < edges.size(); ++i)
        {
            if (s.edgeState[i] == 1)
            {
                const Edge &e = edges[i];
                adj[e.u].push_back(e.v);
                adj[e.v].push_back(e.u);
                onEdges++;
            }
        }
        if (onEdges == 0)
            return false;

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

        {
            lock_guard<mutex> lock(solMutex);
            solutions.push_back(std::move(sol));
            if (!findAll)
            {
                stopAfterFirst.store(true, memory_order_relaxed);
            }
        }
        return true;
    }

    void solveRecursive(State s, int edgeIdx, int depth)
    {
        if (!findAll && stopAfterFirst.load(memory_order_relaxed))
            return;

        // Find next undecided edge
        edgeIdx = selectNextEdge(s, edgeIdx);

        if (edgeIdx == (int)edges.size())
        {
            finalCheckAndStore(s);
            return;
        }

        // Try to determine edge value based on constraints
        bool canBeOn = true;
        bool canBeOff = true;

        {
            State testOn = s;
            canBeOn = applyDecision(testOn, edgeIdx, 1);
        }
        {
            State testOff = s;
            canBeOff = applyDecision(testOff, edgeIdx, -1);
        }

        if (!canBeOn && !canBeOff)
            return; // No valid choice

        if (canBeOn && !canBeOff)
        {
            // Forced to be ON
            if (applyDecision(s, edgeIdx, 1))
                solveRecursive(std::move(s), edgeIdx + 1, depth);
            return;
        }

        if (!canBeOn && canBeOff)
        {
            // Forced to be OFF
            if (applyDecision(s, edgeIdx, -1))
                solveRecursive(std::move(s), edgeIdx + 1, depth);
            return;
        }

        // Both choices possible - branch
        bool shouldParallelize = (depth < maxParallelDepth &&
                                  activeThreads.load(memory_order_relaxed) < maxThreads);

        if (shouldParallelize)
        {
            State sOff = s;
            State sOn = s;

            activeThreads.fetch_add(1, memory_order_relaxed);
            auto fut = std::async(std::launch::async, [this, sOff, edgeIdx, depth]() mutable
                                  {
                if (applyDecision(sOff, edgeIdx, -1))
                    solveRecursive(std::move(sOff), edgeIdx + 1, depth + 1);
                activeThreads.fetch_sub(1, memory_order_relaxed); });

            if (applyDecision(sOn, edgeIdx, 1))
                solveRecursive(std::move(sOn), edgeIdx + 1, depth + 1);

            fut.get();
        }
        else
        {
            // Serial execution
            State sOff = s;
            if (applyDecision(sOff, edgeIdx, -1))
                solveRecursive(std::move(sOff), edgeIdx + 1, depth + 1);

            if (!findAll && stopAfterFirst.load(memory_order_relaxed))
                return;

            if (applyDecision(s, edgeIdx, 1))
                solveRecursive(std::move(s), edgeIdx + 1, depth + 1);
        }
    }

    void run(bool allSolutions)
    {
        findAll = allSolutions;
        stopAfterFirst.store(false, memory_order_relaxed);
        buildEdges();
        auto s = make_unique<State>(initialState());
        solveRecursivePtr(std::move(s), 0, 0);
    }

    void solveRecursivePtr(unique_ptr<State> s, int edgeIdx, int depth)
    {
        if (!findAll && stopAfterFirst.load(memory_order_relaxed))
            return;

        // Apply constraint propagation FIRST
        if (!propagateConstraints(*s))
            return;

        edgeIdx = selectNextEdge(*s, 0); // Search from 0 after propagation

        if (edgeIdx == (int)edges.size())
        {
            finalCheckAndStore(*s);
            return;
        }

        // Test both branches with propagation
        auto sOff = make_unique<State>(*s);
        auto sOn = make_unique<State>(*s);

        bool canBeOff = applyDecision(*sOff, edgeIdx, -1) && propagateConstraints(*sOff);
        bool canBeOn = applyDecision(*sOn, edgeIdx, 1) && propagateConstraints(*sOn);

        if (!canBeOn && !canBeOff)
            return;

        if (canBeOn && !canBeOff)
        {
            solveRecursivePtr(std::move(sOn), edgeIdx + 1, depth);
            return;
        }

        if (!canBeOn && canBeOff)
        {
            solveRecursivePtr(std::move(sOff), edgeIdx + 1, depth);
            return;
        }

        bool shouldParallelize = (depth < maxParallelDepth &&
                                  activeThreads.load(memory_order_relaxed) < maxThreads);

        if (shouldParallelize)
        {
            activeThreads.fetch_add(1, memory_order_relaxed);
            auto fut = std::async(std::launch::async, [this, sOff = std::move(sOff), edgeIdx, depth]() mutable
                                  {
                solveRecursivePtr(std::move(sOff), edgeIdx + 1, depth + 1);
                activeThreads.fetch_sub(1, memory_order_relaxed); });

            solveRecursivePtr(std::move(sOn), edgeIdx + 1, depth + 1);
            fut.get();
        }
        else
        {
            solveRecursivePtr(std::move(sOff), edgeIdx + 1, depth + 1);

            if (!findAll && stopAfterFirst.load(memory_order_relaxed))
                return;

            solveRecursivePtr(std::move(sOn), edgeIdx + 1, depth + 1);
        }
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
            cout << "No solutions found.\n";
            return;
        }
        cout << "Found " << solutions.size() << " solution(s).\n\n";
        for (size_t i = 0; i < solutions.size(); ++i)
        {
            cout << "Solution " << (i + 1) << ":\n";
            printSolution(solutions[i]);
            cout << "\n";
        }
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
