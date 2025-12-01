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

using namespace std;

struct Grid {
    int n = 0, m = 0;
    vector<int> clues; // size n*m, -1 for none

    int cellIndex(int r, int c) const { return r * m + c; }
};

struct Edge {
    int u, v;      // endpoints (point indices)
    int cellA;     // adjacent cell index or -1
    int cellB;     // second adjacent cell index or -1
};

struct State {
    vector<char> edgeState;      // 0 undecided, 1 on, -1 off
    vector<int> pointDegree;     // degree of each point from ON edges
    vector<int> cellEdgeCount;   // count ON edges around each cell
};

struct Solution {
    vector<char> edgeState;
    vector<pair<int,int>> cyclePoints; // ordered cycle as (row,col) on point grid
};

struct Solver {
    Grid grid;
    vector<Edge> edges;
    int numPoints = 0;

    vector<int> horizEdgeIndex;  // (n+1)*m
    vector<int> vertEdgeIndex;   // n*(m+1)

    bool findAll = false;
    atomic<bool> stopAfterFirst{false};

    mutex solMutex;
    vector<Solution> solutions;

    int maxParallelDepth = 8; // bis zu welcher Rekursionstiefe std::async benutzt wird

    void buildEdges() {
        int n = grid.n, m = grid.m;
        numPoints = (n + 1) * (m + 1);
        horizEdgeIndex.assign((n + 1) * m, -1);
        vertEdgeIndex.assign(n * (m + 1), -1);

        edges.clear();
        int idx = 0;

        auto pointId = [this](int r, int c) {
            return r * (grid.m + 1) + c;
        };

        // horizontale Kanten
        for (int r = 0; r <= n; ++r) {
            for (int c = 0; c < m; ++c) {
                Edge e;
                e.u = pointId(r, c);
                e.v = pointId(r, c + 1);
                e.cellA = -1;
                e.cellB = -1;
                if (r > 0) {
                    e.cellA = grid.cellIndex(r - 1, c); // Zelle über der Kante
                }
                if (r < n) {
                    e.cellB = grid.cellIndex(r, c);     // Zelle unter der Kante
                }
                edges.push_back(e);
                horizEdgeIndex[r * m + c] = idx++;
            }
        }

        // vertikale Kanten
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c <= m; ++c) {
                Edge e;
                e.u = pointId(r, c);
                e.v = pointId(r + 1, c);
                e.cellA = -1;
                e.cellB = -1;
                if (c > 0) {
                    e.cellA = grid.cellIndex(r, c - 1); // Zelle links der Kante
                }
                if (c < m) {
                    e.cellB = grid.cellIndex(r, c);     // Zelle rechts der Kante
                }
                edges.push_back(e);
                vertEdgeIndex[r * (m + 1) + c] = idx++;
            }
        }
    }

    State initialState() const {
        State s;
        s.edgeState.assign(edges.size(), 0);
        s.pointDegree.assign(numPoints, 0);
        s.cellEdgeCount.assign(grid.clues.size(), 0);
        return s;
    }

    bool applyDecision(State &s, int edgeIdx, int val) const {
        // val: 1 = ON, -1 = OFF
        if (s.edgeState[edgeIdx] == val) return true;
        if (s.edgeState[edgeIdx] != 0 && s.edgeState[edgeIdx] != val) {
            // conflicting assignment
            return false;
        }
        s.edgeState[edgeIdx] = (char)val;
        const Edge &e = edges[edgeIdx];

        if (val == 1) { // Kante einschalten
            int du = ++s.pointDegree[e.u];
            int dv = ++s.pointDegree[e.v];
            if (du > 2 || dv > 2) return false; // Grad-Bedingung

            auto updateCell = [&](int cellIdx) -> bool {
                if (cellIdx < 0) return true;
                int cnt = ++s.cellEdgeCount[cellIdx];
                int clue = grid.clues[cellIdx];
                if (clue >= 0 && cnt > clue) return false;
                return true;
            };
            if (!updateCell(e.cellA)) return false;
            if (!updateCell(e.cellB)) return false;
        }
        // val == -1: OFF, hier noch keine harte Prüfung nötig
        return true;
    }

    bool finalCheckAndStore(State &s) {
        // Zellen-Bedingungen exakt prüfen
        for (size_t i = 0; i < grid.clues.size(); ++i) {
            int clue = grid.clues[i];
            if (clue >= 0 && s.cellEdgeCount[i] != clue) return false;
        }

        // Adjazenzliste für alle eingeschalteten Kanten
        vector<vector<int>> adj(numPoints);
        int onEdges = 0;
        for (size_t i = 0; i < edges.size(); ++i) {
            if (s.edgeState[i] == 1) {
                const Edge &e = edges[i];
                adj[e.u].push_back(e.v);
                adj[e.v].push_back(e.u);
                onEdges++;
            }
        }
        if (onEdges == 0) return false;

        // Jeder Punkt mit Grad > 0 muss Grad 2 haben
        int start = -1;
        for (int v = 0; v < numPoints; ++v) {
            int deg = s.pointDegree[v];
            if (deg != 0 && deg != 2) return false;
            if (deg == 2 && start == -1) start = v;
        }
        if (start == -1) return false;

        // DFS/BFS: eine zusammenhängende Komponente?
        vector<char> vis(numPoints, 0);
        int visitedVertices = 0;
        int visitedEdges = 0;
        {
            stack<int> st;
            st.push(start);
            vis[start] = 1;
            while (!st.empty()) {
                int v = st.top(); st.pop();
                visitedVertices++;
                for (int to : adj[v]) {
                    visitedEdges++; // Jede Kante wird insgesamt doppelt gezählt
                    if (!vis[to]) {
                        vis[to] = 1;
                        st.push(to);
                    }
                }
            }
        }
        int componentEdges = visitedEdges / 2;

        // Alle Punkte mit Grad 2 müssen in der Komponente sein
        for (int v = 0; v < numPoints; ++v) {
            if (s.pointDegree[v] == 2 && !vis[v]) return false;
        }

        // Komponente muss alle eingeschalteten Kanten enthalten
        if (componentEdges != onEdges) return false;

        // Geschlossenen Zyklus in Reihenfolge aufbauen
        vector<pair<int,int>> cycle;
        {
            int n = grid.n, m = grid.m;
            auto coord = [m](int id) {
                int cols = m + 1;
                return make_pair(id / cols, id % cols);
            };
            int cur = start;
            int prev = -1;
            while (true) {
                cycle.push_back(coord(cur));
                int next = -1;
                for (int to : adj[cur]) {
                    if (to != prev) { 
                        next = to; 
                        break; 
                    }
                }
                if (next == -1) break;
                prev = cur;
                cur = next;
                if (cur == start) {
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
            if (!findAll) {
                stopAfterFirst.store(true, memory_order_relaxed);
            }
        }
        return true;
    }

    void solveRecursive(State s, int edgeIdx, int depth) {
        if (!findAll && stopAfterFirst.load(memory_order_relaxed)) return;

        if (edgeIdx == (int)edges.size()) {
            finalCheckAndStore(s);
            return;
        }

        // (sollte eigentlich nie vorkommen mit diesem Design)
        if (s.edgeState[edgeIdx] != 0) {
            solveRecursive(s, edgeIdx + 1, depth);
            return;
        }

        auto branch = [&](int val, State localState) {
            if (applyDecision(localState, edgeIdx, val)) {
                solveRecursive(std::move(localState), edgeIdx + 1, depth + 1);
            }
        };

        if (depth < maxParallelDepth) {
            // Ein Branch parallel, einer im aktuellen Thread
            State sOff = s;
            State sOn = s;

            auto fut = std::async(std::launch::async, [this, edgeIdx, depth, sOff]() mutable {
                branch(-1, std::move(sOff)); // OFF
            });
            branch(1, std::move(sOn)); // ON
            fut.get();
        } else {
            // nur seriell, um nicht zu viele Threads zu erzeugen
            branch(-1, s);          // OFF
            branch(1, std::move(s)); // ON
        }
    }

    void run(bool allSolutions) {
        findAll = allSolutions;
        stopAfterFirst.store(false, memory_order_relaxed);
        buildEdges();
        State s = initialState();
        solveRecursive(std::move(s), 0, 0);
    }

    void printSolution(const Solution &sol) const {
        int n = grid.n, m = grid.m;
        auto isHorizOn = [&](int r, int c) -> bool {
            int idx = horizEdgeIndex[r * m + c];
            return sol.edgeState[idx] == 1;
        };
        auto isVertOn = [&](int r, int c) -> bool {
            int idx = vertEdgeIndex[r * (m + 1) + c];
            return sol.edgeState[idx] == 1;
        };

        // Zeilen mit Punkten und Horizontal-Kanten
        for (int r = 0; r <= n; ++r) {
            string line;
            for (int c = 0; c < m; ++c) {
                line += "+";
                line += (isHorizOn(r, c) ? "-" : " ");
            }
            line += "+";
            cout << line << "\n";

            if (r == n) break;

            // Zeilen mit Vertikal-Kanten und Ziffern
            string vline;
            for (int c = 0; c < m; ++c) {
                vline += (isVertOn(r, c) ? "|" : " ");
                int clue = grid.clues[grid.cellIndex(r, c)];
                char ch = ' ';
                if (clue >= 0) ch = char('0' + clue);
                vline += ch;
            }
            // rechte Randkante
            vline += (isVertOn(r, m) ? "|" : " ");
            cout << vline << "\n";
        }

        // Zyklus als Liste von Punktkoordinaten
        cout << "Cycle (point coordinates row,col):\n";
        for (size_t i = 0; i < sol.cyclePoints.size(); ++i) {
            auto [r, c] = sol.cyclePoints[i];
            cout << "(" << r << "," << c << ")";
            if (i + 1 < sol.cyclePoints.size()) cout << " -> ";
        }
        cout << "\n";
    }

    void printSolutions() const {
        if (solutions.empty()) {
            cout << "No solutions found.\n";
            return;
        }
        cout << "Found " << solutions.size() << " solution(s).\n\n";
        for (size_t i = 0; i < solutions.size(); ++i) {
            cout << "Solution " << (i + 1) << ":\n";
            printSolution(solutions[i]);
            cout << "\n";
        }
    }
};

Grid readGridFromFile(const string &filename) {
    ifstream in(filename);
    if (!in) {
        throw runtime_error("Could not open file " + filename);
    }
    Grid g;
    in >> g.n >> g.m;
    string line;
    getline(in, line); // Rest der ersten Zeile wegwerfen

    g.clues.assign(g.n * g.m, -1);

    for (int r = 0; r < g.n; ++r) {
        if (!getline(in, line)) {
            throw runtime_error("Not enough grid lines in file");
        }
        if (line.empty()) { r--; continue; } // leere Zeilen überspringen
        vector<int> row;
        for (char ch : line) {
            if (ch == ' ' || ch == '\t') continue;
            if (ch >= '0' && ch <= '3') {
                row.push_back(ch - '0');
            } else {
                row.push_back(-1);
            }
            if ((int)row.size() == g.m) break;
        }
        if ((int)row.size() != g.m) {
            throw runtime_error("Row " + to_string(r) + " does not have m entries");
        }
        for (int c = 0; c < g.m; ++c) {
            g.clues[g.cellIndex(r, c)] = row[c];
        }
    }
    return g;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <inputfile> [--all]\n";
        return 1;
    }
    string filename = argv[1];
    bool allSolutions = false;
    if (argc >= 3) {
        string arg2 = argv[2];
        if (arg2 == "--all") allSolutions = true;
    }

    try {
        Grid g = readGridFromFile(filename);
        Solver solver;
        solver.grid = std::move(g);

        auto start = chrono::steady_clock::now();
        solver.run(allSolutions);
        auto end = chrono::steady_clock::now();
        double seconds = chrono::duration_cast<chrono::duration<double>>(end - start).count();

        solver.printSolutions();
        cout << "Time: " << seconds << " s\n";
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
