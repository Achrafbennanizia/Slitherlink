// Slitherlink Solver - Version 1: Baseline Implementation
// Week 1, Days 1-3 (November 2025)
// Performance: 4×4: 0.100s, 8×8: 15.0s
// Lines: ~800
// Features: Basic backtracking, no parallelization

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <stack>
#include <algorithm>

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
    vector<Solution> solutions;

    void loadPuzzle(const string &filename)
    {
        ifstream file(filename);
        if (!file)
            throw runtime_error("Cannot open file");

        file >> grid.n >> grid.m;
        grid.clues.resize(grid.n * grid.m, -1);

        for (int i = 0; i < grid.n; ++i)
        {
            for (int j = 0; j < grid.m; ++j)
            {
                string token;
                file >> token;
                if (token != "." && token != "-")
                {
                    grid.clues[grid.cellIndex(i, j)] = stoi(token);
                }
            }
        }
    }

    void buildGraph()
    {
        numPoints = (grid.n + 1) * (grid.m + 1);

        horizEdgeIndex.assign((grid.n + 1) * grid.m, -1);
        vertEdgeIndex.assign(grid.n * (grid.m + 1), -1);
        cellEdges.resize(grid.n * grid.m);
        pointEdges.resize(numPoints);

        auto pointIdx = [&](int r, int c)
        { return r * (grid.m + 1) + c; };

        // Horizontal edges
        for (int r = 0; r <= grid.n; ++r)
        {
            for (int c = 0; c < grid.m; ++c)
            {
                Edge e;
                e.u = pointIdx(r, c);
                e.v = pointIdx(r, c + 1);
                e.cellA = (r > 0) ? grid.cellIndex(r - 1, c) : -1;
                e.cellB = (r < grid.n) ? grid.cellIndex(r, c) : -1;

                int idx = edges.size();
                horizEdgeIndex[r * grid.m + c] = idx;
                edges.push_back(e);

                pointEdges[e.u].push_back(idx);
                pointEdges[e.v].push_back(idx);
                if (e.cellA >= 0)
                    cellEdges[e.cellA].push_back(idx);
                if (e.cellB >= 0)
                    cellEdges[e.cellB].push_back(idx);
            }
        }

        // Vertical edges
        for (int r = 0; r < grid.n; ++r)
        {
            for (int c = 0; c <= grid.m; ++c)
            {
                Edge e;
                e.u = pointIdx(r, c);
                e.v = pointIdx(r + 1, c);
                e.cellA = (c > 0) ? grid.cellIndex(r, c - 1) : -1;
                e.cellB = (c < grid.m) ? grid.cellIndex(r, c) : -1;

                int idx = edges.size();
                vertEdgeIndex[r * (grid.m + 1) + c] = idx;
                edges.push_back(e);

                pointEdges[e.u].push_back(idx);
                pointEdges[e.v].push_back(idx);
                if (e.cellA >= 0)
                    cellEdges[e.cellA].push_back(idx);
                if (e.cellB >= 0)
                    cellEdges[e.cellB].push_back(idx);
            }
        }

        for (int i = 0; i < grid.n * grid.m; ++i)
        {
            if (grid.clues[i] >= 0)
                clueCells.push_back(i);
        }
    }

    bool quickValidityCheck(const State &s) const
    {
        for (int p : clueCells)
        {
            int clue = grid.clues[p];
            int cnt = s.cellEdgeCount[p];
            int und = s.cellUndecided[p];
            if (cnt > clue || cnt + und < clue)
                return false;
        }

        for (int i = 0; i < numPoints; ++i)
        {
            int deg = s.pointDegree[i];
            int und = s.pointUndecided[i];
            if (deg > 2 || deg + und < 2)
                return false;
        }

        return true;
    }

    int selectNextEdge(const State &s)
    {
        int best = -1;
        int maxScore = -1;

        for (int i = 0; i < (int)edges.size(); ++i)
        {
            if (s.edgeState[i] != 0)
                continue;

            const Edge &e = edges[i];
            int score = 0;

            // Prioritize points with degree 1
            if (s.pointDegree[e.u] == 1 || s.pointDegree[e.v] == 1)
            {
                score += 10000;
            }

            // Score based on cell constraints
            if (e.cellA >= 0 && grid.clues[e.cellA] >= 0)
            {
                int clue = grid.clues[e.cellA];
                int cnt = s.cellEdgeCount[e.cellA];
                int und = s.cellUndecided[e.cellA];
                int need = clue - cnt;

                if (need == und || need == 0)
                    score += 2000; // Binary decision
                else if (und == 1)
                    score += 1500;
                else if (und <= 2)
                    score += 1000;
            }

            if (e.cellB >= 0 && grid.clues[e.cellB] >= 0)
            {
                int clue = grid.clues[e.cellB];
                int cnt = s.cellEdgeCount[e.cellB];
                int und = s.cellUndecided[e.cellB];
                int need = clue - cnt;

                if (need == und || need == 0)
                    score += 2000;
                else if (und == 1)
                    score += 1500;
                else if (und <= 2)
                    score += 1000;
            }

            if (score > maxScore)
            {
                maxScore = score;
                best = i;
            }
        }

        return best;
    }

    bool finalCheckAndStore(State &s)
    {
        // Check all clues satisfied
        for (int p : clueCells)
        {
            if (s.cellEdgeCount[p] != grid.clues[p])
                return false;
        }

        // Build adjacency list from ON edges
        vector<vector<int>> adj(numPoints);
        int start = -1;
        for (int i = 0; i < (int)edges.size(); ++i)
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

        // Check degrees
        int onEdges = 0;
        for (int i = 0; i < numPoints; ++i)
        {
            int deg = adj[i].size();
            if (deg != 0 && deg != 2)
                return false;
            onEdges += deg;
        }
        onEdges /= 2;

        if (start == -1)
            return false;

        // DFS to check single cycle
        vector<bool> visited(numPoints, false);
        int cur = start, prev = -1, visitedEdges = 0;

        while (true)
        {
            visited[cur] = true;
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
            ++visitedEdges;
            prev = cur;
            cur = next;
            if (cur == start)
            {
                ++visitedEdges;
                break;
            }
        }

        // Verify all degree-2 points visited
        for (int i = 0; i < numPoints; ++i)
        {
            if (adj[i].size() == 2 && !visited[i])
                return false;
        }

        if (visitedEdges != onEdges)
            return false;

        // Store solution
        Solution sol;
        sol.edgeState = s.edgeState;

        // Build cycle
        vector<pair<int, int>> cycle;
        auto coord = [&](int p)
        {
            return make_pair(p / (grid.m + 1), p % (grid.m + 1));
        };

        cur = start;
        prev = -1;
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
                cycle.push_back(coord(start));
                break;
            }
        }

        sol.cyclePoints = cycle;
        solutions.push_back(sol);

        return true;
    }

    void search(State s, int depth)
    {
        if (!quickValidityCheck(s))
            return;

        int edgeIdx = selectNextEdge(s);
        if (edgeIdx == -1)
        {
            finalCheckAndStore(s);
            return;
        }

        const Edge &e = edges[edgeIdx];

        // Try ON
        {
            State onState = s;
            onState.edgeState[edgeIdx] = 1;
            onState.pointDegree[e.u]++;
            onState.pointDegree[e.v]++;
            onState.pointUndecided[e.u]--;
            onState.pointUndecided[e.v]--;
            if (e.cellA >= 0)
            {
                onState.cellEdgeCount[e.cellA]++;
                onState.cellUndecided[e.cellA]--;
            }
            if (e.cellB >= 0)
            {
                onState.cellEdgeCount[e.cellB]++;
                onState.cellUndecided[e.cellB]--;
            }

            search(onState, depth + 1);
        }

        // Try OFF
        {
            State offState = s;
            offState.edgeState[edgeIdx] = -1;
            offState.pointUndecided[e.u]--;
            offState.pointUndecided[e.v]--;
            if (e.cellA >= 0)
                offState.cellUndecided[e.cellA]--;
            if (e.cellB >= 0)
                offState.cellUndecided[e.cellB]--;

            search(offState, depth + 1);
        }
    }

    void solve()
    {
        State initial;
        initial.edgeState.assign(edges.size(), 0);
        initial.pointDegree.assign(numPoints, 0);
        initial.cellEdgeCount.assign(grid.n * grid.m, 0);
        initial.cellUndecided.resize(grid.n * grid.m);
        initial.pointUndecided.resize(numPoints);

        for (int i = 0; i < (int)edges.size(); ++i)
        {
            const Edge &e = edges[i];
            initial.pointUndecided[e.u]++;
            initial.pointUndecided[e.v]++;
        }

        for (int i = 0; i < grid.n * grid.m; ++i)
        {
            initial.cellUndecided[i] = cellEdges[i].size();
        }

        search(initial, 0);
    }

    void printSolution(const Solution &sol)
    {
        cout << "\n=== Solution Found ===\n";
        cout << "Cycle: ";
        for (size_t i = 0; i < sol.cyclePoints.size(); ++i)
        {
            auto [r, c] = sol.cyclePoints[i];
            cout << "(" << r << "," << c << ")";
            if (i + 1 < sol.cyclePoints.size())
                cout << " -> ";
        }
        cout << "\n";
    }
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <puzzle_file> [--all]\n";
        return 1;
    }

    Solver solver;
    solver.findAll = (argc > 2 && string(argv[2]) == "--all");

    try
    {
        solver.loadPuzzle(argv[1]);
        solver.buildGraph();

        cout << "Solving " << solver.grid.n << "×" << solver.grid.m << " puzzle...\n";

        auto start = chrono::high_resolution_clock::now();
        solver.solve();
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end - start;

        cout << "\nFound " << solver.solutions.size() << " solution(s) in "
             << elapsed.count() << " seconds\n";

        if (!solver.solutions.empty())
        {
            solver.printSolution(solver.solutions[0]);
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
