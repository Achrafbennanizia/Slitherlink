/**
 * @file main_solid.cpp
 * @brief Example usage of SOLID architecture Slitherlink solver
 *
 * This demonstrates how to use the refactored solver with dependency injection
 * and the strategy pattern for different algorithms.
 */

#include "include/Grid.h"
#include "include/Edge.h"
#include "include/State.h"
#include "include/Solution.h"
#include "include/Solver.h"
#include "include/SmartHeuristic.h"
#include "include/StandardValidator.h"
#include "include/OptimizedPropagator.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <memory>

using namespace std;
using namespace slitherlink;

// Helper function to build edge graph
void buildEdgeGraph(const Grid &grid,
                    vector<Edge> &edges,
                    vector<vector<int>> &adjacentEdges,
                    vector<vector<int>> &pointEdges)
{
    int rows = grid.getRows();
    int cols = grid.getCols();
    int numCells = rows * cols;
    int numPoints = (rows + 1) * (cols + 1);

    adjacentEdges.resize(numCells);
    pointEdges.resize(numPoints);

    // Build horizontal edges
    for (int r = 0; r <= rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            int u = r * (cols + 1) + c;
            int v = u + 1;

            int cellA = (r > 0) ? (r - 1) * cols + c : -1;
            int cellB = (r < rows) ? r * cols + c : -1;

            int edgeIdx = edges.size();
            edges.push_back({u, v, cellA, cellB});

            if (cellA >= 0)
                adjacentEdges[cellA].push_back(edgeIdx);
            if (cellB >= 0)
                adjacentEdges[cellB].push_back(edgeIdx);

            pointEdges[u].push_back(edgeIdx);
            pointEdges[v].push_back(edgeIdx);
        }
    }

    // Build vertical edges
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c <= cols; ++c)
        {
            int u = r * (cols + 1) + c;
            int v = u + (cols + 1);

            int cellA = (c > 0) ? r * cols + (c - 1) : -1;
            int cellB = (c < cols) ? r * cols + c : -1;

            int edgeIdx = edges.size();
            edges.push_back({u, v, cellA, cellB});

            if (cellA >= 0)
                adjacentEdges[cellA].push_back(edgeIdx);
            if (cellB >= 0)
                adjacentEdges[cellB].push_back(edgeIdx);

            pointEdges[u].push_back(edgeIdx);
            pointEdges[v].push_back(edgeIdx);
        }
    }
}

// Print solution
void printSolution(const Grid &grid, const Solution &sol, const vector<Edge> &edges)
{
    int rows = grid.getRows();
    int cols = grid.getCols();

    // Build adjacency for visualization
    vector<vector<bool>> horiz(rows + 1, vector<bool>(cols, false));
    vector<vector<bool>> vert(rows, vector<bool>(cols + 1, false));

    for (size_t i = 0; i < edges.size(); ++i)
    {
        if (sol.edgeState[i] != 1)
            continue;

        const Edge &e = edges[i];
        int u = e.u;
        int v = e.v;

        // Horizontal edge
        if (abs(u - v) == 1)
        {
            int r = u / (cols + 1);
            int c = min(u, v) % (cols + 1);
            horiz[r][c] = true;
        }
        // Vertical edge
        else
        {
            int c = u % (cols + 1);
            int r = min(u, v) / (cols + 1);
            vert[r][c] = true;
        }
    }

    // Print grid
    for (int r = 0; r <= rows; ++r)
    {
        // Print horizontal edges
        for (int c = 0; c < cols; ++c)
        {
            cout << "+";
            cout << (horiz[r][c] ? "---" : "   ");
        }
        cout << "+\n";

        if (r < rows)
        {
            // Print cells and vertical edges
            for (int c = 0; c <= cols; ++c)
            {
                cout << (vert[r][c] ? "|" : " ");
                if (c < cols)
                {
                    int clue = grid.getClue(r, c);
                    if (clue >= 0)
                    {
                        cout << " " << clue << " ";
                    }
                    else
                    {
                        cout << "   ";
                    }
                }
            }
            cout << "\n";
        }
    }
}

int main(int argc, char *argv[])
{
    string filename = "example4x4.txt";
    bool findAll = false;
    int threads = 1;

    // Parse command line
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "--all")
        {
            findAll = true;
        }
        else if (arg == "--threads" && i + 1 < argc)
        {
            threads = stoi(argv[++i]);
        }
        else
        {
            filename = arg;
        }
    }

    cout << "SOLID Architecture Slitherlink Solver\n";
    cout << "======================================\n\n";

    // Step 1: Load puzzle
    Grid grid;
    if (!grid.loadFromFile(filename))
    {
        cerr << "Failed to load puzzle from: " << filename << "\n";
        return 1;
    }

    cout << "Loaded " << grid.getRows() << "×" << grid.getCols() << " puzzle\n";

    // Step 2: Build edge graph
    vector<Edge> edges;
    vector<vector<int>> adjacentEdges;
    vector<vector<int>> pointEdges;

    buildEdgeGraph(grid, edges, adjacentEdges, pointEdges);
    cout << "Built graph with " << edges.size() << " edges\n\n";

    // Step 3: Create strategy implementations (Dependency Injection)
    auto heuristic = make_unique<SmartHeuristic>(grid, edges, adjacentEdges, pointEdges);
    auto validator = make_unique<StandardValidator>(grid, edges, adjacentEdges, pointEdges);
    auto propagator = make_unique<OptimizedPropagator>(grid, edges, adjacentEdges, pointEdges);

    // Step 4: Configure solver
    SolverConfig config;
    config.findAll = findAll;
    config.threads = threads;
    config.verbose = true;
    config.enableParallel = (threads > 1);

    // Step 5: Create solver with dependency injection
    Solver solver(grid, edges, adjacentEdges, pointEdges,
                  std::move(heuristic),
                  std::move(validator),
                  std::move(propagator),
                  config);

    // Step 6: Solve
    cout << "Solving" << (findAll ? " (finding all solutions)" : "") << "...\n";
    auto start = chrono::high_resolution_clock::now();

    auto solutions = solver.solve();

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    // Step 7: Print results
    cout << "\n========================================\n";
    cout << "Found " << solutions.size() << " solution(s)\n";
    cout << "Time: " << duration.count() / 1000.0 << "s\n";
    cout << "========================================\n\n";

    if (!solutions.empty())
    {
        cout << "First solution:\n";
        printSolution(grid, solutions[0], edges);
    }

    return 0;
}
