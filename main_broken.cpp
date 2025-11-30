#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <stack>
#include <chrono>
#include <memory>
#include <algorithm>
#include <numeric>
#include <thread>

#ifdef USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>
#include <tbb/parallel_reduce.h>
#include <tbb/parallel_scan.h>
#include <tbb/blocked_range.h>
#include <tbb/task_arena.h>
#include <tbb/task_group.h>
#include <tbb/concurrent_vector.h>
#endif

using namespace std;

struct Grid
{
    int rows, cols;
    vector<int> clues;
    int cellIndex(int r, int c) const { return r * cols + c; }
};

struct Edge
{
    int u, v;
    int cellA, cellB;
};

struct State
{
    vector<char> edge;
    vector<int> degree;
    vector<int> cellOn;
    vector<int> cellUnd;
    vector<int> vertUnd;
};

struct Solution
{
    vector<char> edge;
    vector<pair<int, int>> cycle;
};

struct Solver
{
    Grid grid;
    vector<Edge> edges;
    int numVerts = 0;

    vector<int> hEdge, vEdge;
    vector<vector<int>> cellEdges;
    vector<vector<int>> vertEdges;
    vector<int> clueCells;

    bool findAll = false;
    volatile bool stopSearch = false;

#ifdef USE_TBB
    unique_ptr<tbb::task_arena> arena;
    tbb::concurrent_vector<Solution> solutions;
#else
    vector<Solution> solutions;
#endif

    int maxDepth = 8;

    void buildEdges()
    {
        int n = grid.rows, m = grid.cols;
        numVerts = (n + 1) * (m + 1);

        auto vid = [m](int r, int c)
        { return r * (m + 1) + c; };

        hEdge.assign((n + 1) * m, -1);
        vEdge.assign(n * (m + 1), -1);
        cellEdges.assign(n * m, {});
        vertEdges.assign(numVerts, {});
        edges.clear();
        clueCells.clear();
        clueCells.reserve(grid.cells());

        int idx = 0;

        for (int r = 0; r <= n; r++)
        {
            for (int c = 0; c < m; c++)
            {
                Edge e{vid(r, c), vid(r, c + 1), -1, -1};
                if (r > 0)
                    e.cellA = grid.cellIndex(r - 1, c);
                if (r < n)
                    e.cellB = grid.cellIndex(r, c);

                edges.push_back(e);
                hEdge[r * m + c] = idx;

                if (e.cellA >= 0)
                    cellEdges[e.cellA].push_back(idx);
                if (e.cellB >= 0)
                    cellEdges[e.cellB].push_back(idx);
                vertEdges[e.u].push_back(idx);
                vertEdges[e.v].push_back(idx);
                idx++;
            }
        }

        for (int r = 0; r < n; r++)
        {
            for (int c = 0; c <= m; c++)
            {
                Edge e{vid(r, c), vid(r + 1, c), -1, -1};
                if (c > 0)
                    e.cellA = grid.cellIndex(r, c - 1);
                if (c < m)
                    e.cellB = grid.cellIndex(r, c);

                edges.push_back(e);
                vEdge[r * (m + 1) + c] = idx;

                if (e.cellA >= 0)
                    cellEdges[e.cellA].push_back(idx);
                if (e.cellB >= 0)
                    cellEdges[e.cellB].push_back(idx);
                vertEdges[e.u].push_back(idx);
                vertEdges[e.v].push_back(idx);
                idx++;
            }
        }

        for (int i = 0; i < grid.cells(); ++i)
        {
            if (grid.clues[i] >= 0)
                clueCells.push_back(i);
        }
    }

    State initialState() const
    {
        State s;
        s.edge.assign(edges.size(), 0);
        s.degree.assign(numVerts, 0);
        s.cellOn.assign(grid.clues.size(), 0);
        s.cellUnd.assign(grid.clues.size(), 0);
        s.vertUnd.assign(numVerts, 0);

#ifdef USE_TBB
        vector<int> cellIdx(cellEdges.size()), vertIdx(numVerts);
        iota(cellIdx.begin(), cellIdx.end(), 0);
        iota(vertIdx.begin(), vertIdx.end(), 0);

        tbb::parallel_for_each(cellIdx, [&](int i)
                               { s.cellUnd[i] = cellEdges[i].size(); });
        tbb::parallel_for_each(vertIdx, [&](int i)
                               { s.vertUnd[i] = vertEdges[i].size(); });
#else
        for (size_t i = 0; i < cellEdges.size(); i++)
            s.cellUnd[i] = cellEdges[i].size();
        for (int i = 0; i < numVerts; i++)
            s.vertUnd[i] = vertEdges[i].size();
#endif
        return s;
    }

    bool applyDecision(State &s, int ei, int val) const
    {
        if (s.edge[ei] == val)
            return true;
        if (s.edge[ei] != 0)
            return false;

        s.edge[ei] = val;
        const Edge &e = edges[ei];

        s.vertUnd[e.u]--;
        s.vertUnd[e.v]--;
        if (e.cellA >= 0)
            s.cellUnd[e.cellA]--;
        if (e.cellB >= 0)
            s.cellUnd[e.cellB]--;

        if (val == 1)
        {
            if (++s.degree[e.u] > 2 || ++s.degree[e.v] > 2)
                return false;

            if (e.cellA >= 0 && grid.clues[e.cellA] >= 0)
            {
                if (++s.cellOn[e.cellA] > grid.clues[e.cellA])
                    return false;
            }
            if (e.cellB >= 0 && grid.clues[e.cellB] >= 0)
            {
                if (++s.cellOn[e.cellB] > grid.clues[e.cellB])
                    return false;
            }
        }
        return true;
    }

    bool quickCheck(const State &s) const
    {
#ifdef USE_TBB
        bool vertOk = tbb::parallel_reduce(
            tbb::blocked_range<int>(0, numVerts), true,
            [&](const tbb::blocked_range<int> &r, bool ok)
            {
                for (int i = r.begin(); i < r.end() && ok; i++)
                {
                    if (s.degree[i] > 2)
                        ok = false;
                    if (s.degree[i] == 1 && s.vertUnd[i] == 0)
                        ok = false;
                }
                return ok;
            },
            [](bool a, bool b)
            { return a && b; });
        if (!vertOk)
            return false;

        return tbb::parallel_reduce(
            tbb::blocked_range<size_t>(0, clueCells.size()), true,
            [&](const tbb::blocked_range<size_t> &r, bool ok)
            {
                for (size_t i = r.begin(); i < r.end() && ok; i++)
                {
                    int cell = clueCells[i];
                    int clue = grid.clues[cell];
                    if (s.cellOn[cell] > clue)
                        ok = false;
                    if (s.cellOn[cell] + s.cellUnd[cell] < clue)
                        ok = false;
                }
                return ok;
            },
            [](bool a, bool b)
            { return a && b; });
#else
        for (int i = 0; i < numVerts; i++)
        {
            if (s.degree[i] > 2)
                return false;
            if (s.degree[i] == 1 && s.vertUnd[i] == 0)
                return false;
        }
        for (int cell : clueCells)
        {
            int clue = grid.clues[cell];
            if (s.cellOn[cell] > clue)
                return false;
            if (s.cellOn[cell] + s.cellUnd[cell] < clue)
                return false;
        }
        return true;
#endif
    }

    bool propagate(State &s) const
    {
        for (int cell : clueCells)
        {
            int clue = grid.clues[cell];
            if (s.cellOn[cell] > clue || s.cellOn[cell] + s.cellUnd[cell] < clue)
                return false;
        }

        vector<int> cellQ, vertQ;
        vector<bool> cellQueued(grid.clues.size(), false);
        vector<bool> vertQueued(numVerts, false);

        for (int cell : clueCells)
        {
            cellQ.push_back(cell);
            cellQueued[cell] = true;
        }
        for (int i = 0; i < numVerts; i++)
        {
            vertQ.push_back(i);
            vertQueued[i] = true;
        }

        size_t cPos = 0, vPos = 0;

        auto queueEdge = [&](int ei)
        {
            const Edge &e = edges[ei];
            if (e.cellA >= 0 && !cellQueued[e.cellA] && grid.clues[e.cellA] >= 0)
            {
                cellQ.push_back(e.cellA);
                cellQueued[e.cellA] = true;
            }
            if (e.cellB >= 0 && !cellQueued[e.cellB] && grid.clues[e.cellB] >= 0)
            {
                cellQ.push_back(e.cellB);
                cellQueued[e.cellB] = true;
            }
            if (!vertQueued[e.u])
            {
                vertQ.push_back(e.u);
                vertQueued[e.u] = true;
            }
            if (!vertQueued[e.v])
            {
                vertQ.push_back(e.v);
                vertQueued[e.v] = true;
            }
        };

        while (cPos < cellQ.size() || vPos < vertQ.size())
        {
            while (cPos < cellQ.size())
            {
                int ci = cellQ[cPos++];
                cellQueued[ci] = false;

                int clue = grid.clues[ci];
                if (clue < 0)
                    continue;

                int need = clue - s.cellOn[ci];

                if (need == s.cellUnd[ci])
                {
                    for (int ei : cellEdges[ci])
                    {
                        if (s.edge[ei] == 0)
                        {
                            if (!applyDecision(s, ei, 1))
                                return false;
                            queueEdge(ei);
                        }
                    }
                }
                else if (need == 0 && s.cellUnd[ci] > 0)
                {
                    for (int ei : cellEdges[ci])
                    {
                        if (s.edge[ei] == 0)
                        {
                            s.edge[ei] = -1;
                            const Edge &e = edges[ei];
                            s.vertUnd[e.u]--;
                            s.vertUnd[e.v]--;
                            if (e.cellA >= 0)
                                s.cellUnd[e.cellA]--;
                            if (e.cellB >= 0)
                                s.cellUnd[e.cellB]--;

                            if (!vertQueued[e.u])
                            {
                                vertQ.push_back(e.u);
                                vertQueued[e.u] = true;
                            }
                            if (!vertQueued[e.v])
                            {
                                vertQ.push_back(e.v);
                                vertQueued[e.v] = true;
                            }
                        }
                    }
                }
            }

            while (vPos < vertQ.size())
            {
                int vi = vertQ[vPos++];
                vertQueued[vi] = false;

                int deg = s.degree[vi];
                int und = s.vertUnd[vi];

                if (deg >= 2 || (deg == 0 && und == 0))
                    continue;

                if (deg == 1 && und == 1)
                {
                    for (int ei : vertEdges[vi])
                    {
                        if (s.edge[ei] == 0)
                        {
                            if (!applyDecision(s, ei, 1))
                                return false;
                            queueEdge(ei);
                        }
                    }
                }
                else if (deg == 2 && und > 0)
                {
                    for (int ei : vertEdges[vi])
                    {
                        if (s.edge[ei] == 0)
                        {
                            s.edge[ei] = -1;
                            const Edge &e = edges[ei];
                            s.vertUnd[e.u]--;
                            s.vertUnd[e.v]--;
                            if (e.cellA >= 0)
                                s.cellUnd[e.cellA]--;
                            if (e.cellB >= 0)
                                s.cellUnd[e.cellB]--;

                            int other = (e.u == vi) ? e.v : e.u;
                            if (!vertQueued[other])
                            {
                                vertQ.push_back(other);
                                vertQueued[other] = true;
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

    int selectEdge(const State &s) const
    {
        int best = -1, bestScore = -1000;

        for (int i = 0; i < (int)edges.size(); i++)
        {
            if (s.edge[i] != 0)
                continue;

            const Edge &e = edges[i];
            int score = 0;

            if (s.degree[e.u] == 1)
                score += 10000;
            if (s.degree[e.v] == 1)
                score += 10000;

            if (s.degree[e.u] == 0 && s.vertUnd[e.u] == 2)
                score += 5000;
            if (s.degree[e.v] == 0 && s.vertUnd[e.v] == 2)
                score += 5000;

            auto scoreCell = [&](int ci)
            {
                if (ci >= 0 && grid.clues[ci] >= 0)
                {
                    int need = grid.clues[ci] - s.cellOn[ci];
                    if (need == s.cellUnd[ci] || need == 0)
                        score += 2000;
                    else if (s.cellUnd[ci] == 1)
                        score += 1500;
                    else if (s.cellUnd[ci] <= 2)
                        score += 1000;
                }
            };
            scoreCell(e.cellA);
            scoreCell(e.cellB);

            if (score > bestScore)
            {
                bestScore = score;
                best = i;
                if (score >= 10000)
                    return best;
            }
        }

        return best >= 0 ? best : (int)edges.size();
    }

    bool checkAndStore(State &s)
    {
        for (int cell : clueCells)
        {
            if (s.cellOn[cell] != grid.clues[cell])
                return false;
        }

#ifdef USE_TBB
        vector<int> prefix(edges.size());
        int onCount = tbb::parallel_scan(
            tbb::blocked_range<size_t>(0, edges.size()), 0,
            [&](const tbb::blocked_range<size_t> &r, int sum, bool isFinal)
            {
                for (size_t i = r.begin(); i < r.end(); i++)
                {
                    sum += (s.edge[i] == 1);
                    if (isFinal)
                        prefix[i] = sum;
                }
                return sum;
            },
            std::plus<int>());
#else
        vector<int> prefix(edges.size());
        int onCount = 0;
        for (size_t i = 0; i < edges.size(); i++)
        {
            onCount += (s.edge[i] == 1);
            prefix[i] = onCount;
        }
#endif
        if (onCount == 0)
            return false;

        vector<vector<int>> adj(numVerts);
        for (int i = 0; i < (int)edges.size(); i++)
        {
            if (s.edge[i] == 1)
            {
                const Edge &e = edges[i];
                adj[e.u].push_back(e.v);
                adj[e.v].push_back(e.u);
            }
        }

        int start = -1;
        for (int v = 0; v < numVerts; v++)
        {
            if (s.degree[v] != 0 && s.degree[v] != 2)
                return false;
            if (s.degree[v] == 2 && start == -1)
                start = v;
        }
        if (start == -1)
            return false;

        vector<bool> vis(numVerts, false);
        int visitedEdges = 0;
        stack<int> st;
        st.push(start);
        vis[start] = true;

        while (!st.empty())
        {
            int v = st.top();
            st.pop();
            for (int u : adj[v])
            {
                visitedEdges++;
                if (!vis[u])
                {
                    vis[u] = true;
                    st.push(u);
                }
            }
        }

        for (int v = 0; v < numVerts; v++)
        {
            if (s.degree[v] == 2 && !vis[v])
                return false;
        }

        if (visitedEdges / 2 != onCount)
            return false;

        vector<pair<int, int>> cycle;
        auto coord = [this](int v)
        {
            return make_pair(v / (grid.cols + 1), v % (grid.cols + 1));
        };

        int cur = start, prev = -1;
        while (true)
        {
            cycle.push_back(coord(cur));
            int next = -1;
            for (int u : adj[cur])
            {
                if (u != prev)
                {
                    next = u;
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

        Solution sol{s.edge, cycle};

#ifdef USE_TBB
        solutions.push_back(sol);
        if (!findAll)
            stopSearch = true;
#else
        solutions.push_back(sol);
        if (!findAll)
            stopSearch = true;
#endif
        return true;
    }

    void solve(unique_ptr<State> s, int depth)
    {
        if (!findAll && stopSearch)
            return;

        if (!quickCheck(*s))
            return;
        if (!propagate(*s))
            return;

        int ei = selectEdge(*s);

        if (ei == (int)edges.size())
        {
            checkAndStore(*s);
            return;
        }

        bool canOff = true, canOn = true;

        {
            int du = s->degree[edges[ei].u];
            int dv = s->degree[edges[ei].v];
            int uu = s->vertUnd[edges[ei].u];
            int uv = s->vertUnd[edges[ei].v];

            if ((du == 1 && uu == 1) || (dv == 1 && uv == 1))
                canOff = false;
            if (du >= 2 || dv >= 2)
                canOn = false;
        }

        auto sOff = canOff ? make_unique<State>(*s) : nullptr;
        auto sOn = canOn ? make_unique<State>(*s) : nullptr;

        if (canOff)
            canOff = applyDecision(*sOff, ei, -1) && quickCheck(*sOff) && propagate(*sOff);
        if (canOn)
            canOn = applyDecision(*sOn, ei, 1) && quickCheck(*sOn) && propagate(*sOn);

        if (!canOn && !canOff)
            return;
        if (canOn && !canOff)
        {
            solve(std::move(sOn), depth);
            return;
        }
        if (!canOn && canOff)
        {
            solve(std::move(sOff), depth);
            return;
        }

#ifdef USE_TBB
        if (depth < maxDepth)
        {
            tbb::task_group g;

            auto offPtr = sOff.release();
            g.run([this, offPtr, depth]()
                  {
                unique_ptr<State> s(offPtr);
                solve(std::move(s), depth+1); });

            solve(std::move(sOn), depth + 1);
            g.wait();
        }
        else
        {
            solve(std::move(sOff), depth + 1);
            if (!findAll && stopSearch)
                return;
            solve(std::move(sOn), depth + 1);
        }
#else
        solve(std::move(sOff), depth + 1);
        if (!findAll && stopSearch)
            return;
        solve(std::move(sOn), depth + 1);
#endif
    }

    void run(bool allSolutions)
    {
        findAll = allSolutions;
        stopSearch = false;

        cout << "Searching for " << (allSolutions ? "all solutions" : "first solution") << "...\n";

        buildEdges();
        auto s = make_unique<State>(initialState());

#ifdef USE_TBB
        int threads = max(1, (int)thread::hardware_concurrency() / 2);
        cout << "Using Intel oneAPI TBB with " << threads << " threads (50% of available cores)\n";
        arena = make_unique<tbb::task_arena>(threads);
        solutions.clear();

        auto sPtr = s.release();
        arena->execute([this, sPtr]()
                       {
            unique_ptr<State> state(sPtr);
            solve(std::move(state), 0); });
#else
        solve(std::move(s), 0);
#endif
    }

    void printSolution(const Solution &sol) const
    {
        auto isHOn = [&](int r, int c)
        {
            return sol.edge[hEdge[r * grid.cols + c]] == 1;
        };
        auto isVOn = [&](int r, int c)
        {
            return sol.edge[vEdge[r * (grid.cols + 1) + c]] == 1;
        };

        for (int r = 0; r <= grid.rows; r++)
        {
            for (int c = 0; c < grid.cols; c++)
                cout << "+" << (isHOn(r, c) ? "-" : " ");
            cout << "+\n";

            if (r == grid.rows)
                break;

            for (int c = 0; c < grid.cols; c++)
            {
                cout << (isVOn(r, c) ? "|" : " ");
                int clue = grid.clues[grid.cellIndex(r, c)];
                cout << (clue >= 0 ? char('0' + clue) : ' ');
            }
            cout << (isVOn(r, grid.cols) ? "|" : " ") << "\n";
        }

        cout << "Cycle: ";
        for (size_t i = 0; i < sol.cycle.size(); i++)
        {
            auto [r, c] = sol.cycle[i];
            cout << "(" << r << "," << c << ")";
            if (i + 1 < sol.cycle.size())
                cout << " -> ";
        }
        cout << "\n";
    }

    void printSummary() const
    {
        if (solutions.empty())
        {
            cout << "\nNo solutions found.\n";
        }
        else
        {
            for (size_t i = 0; i < solutions.size(); i++)
            {
                cout << "\n=== Solution " << (i + 1) << " ===\n";
                printSolution(solutions[i]);
            }
            cout << "\n=== SUMMARY ===\n";
            cout << "Total solutions: " << solutions.size() << "\n";
        }
    }
};

Grid readGrid(const string &filename)
{
    ifstream in(filename);
    if (!in)
        throw runtime_error("Cannot open file: " + filename);

    Grid g;
    in >> g.rows >> g.cols;
    g.clues.assign(g.rows * g.cols, -1);

    string line;
    getline(in, line);

    for (int r = 0; r < g.rows; r++)
    {
        if (!getline(in, line))
            throw runtime_error("Missing grid row");
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
            row.push_back((ch >= '0' && ch <= '3') ? (ch - '0') : -1);
            if ((int)row.size() == g.cols)
                break;
        }

        if ((int)row.size() != g.cols)
            throw runtime_error("Row " + to_string(r) + " has wrong column count");

        for (int c = 0; c < g.cols; c++)
            g.clues[g.cellIndex(r, c)] = row[c];
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

    bool allSolutions = (argc >= 3 && string(argv[2]) == "--all");

    try
    {
        Solver solver;
        solver.grid = readGrid(argv[1]);

        auto start = chrono::steady_clock::now();
        solver.run(allSolutions);
        auto end = chrono::steady_clock::now();

        solver.printSummary();

        double seconds = chrono::duration<double>(end - start).count();
        cout << "Time: " << seconds << " s\n";
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
