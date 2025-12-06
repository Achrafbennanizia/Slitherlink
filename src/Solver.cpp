#include "Solver.h"
#include <algorithm>
#include <stack>
#include <iostream>

#ifdef USE_TBB
#include <tbb/task_group.h>
#include <tbb/parallel_for.h>
#endif

using namespace slitherlink;

std::vector<Solution> Solver::solve()
{
    solutions.clear();
    foundSolution.store(false);

    // Initialize state
    State initialState;
    int numPoints = (grid.getRows() + 1) * (grid.getCols() + 1);
    int numCells = grid.getRows() * grid.getCols();

    initialState.initialize(edges.size(), numPoints, numCells);

    // Initialize edge counts
    for (size_t i = 0; i < edges.size(); ++i)
    {
        const Edge &e = edges[i];
        initialState.setPointUndecided(e.u, initialState.getPointUndecided(e.u) + 1);
        initialState.setPointUndecided(e.v, initialState.getPointUndecided(e.v) + 1);

        if (e.cellA >= 0)
        {
            initialState.setCellUndecided(e.cellA, initialState.getCellUndecided(e.cellA) + 1);
        }
        if (e.cellB >= 0)
        {
            initialState.setCellUndecided(e.cellB, initialState.getCellUndecided(e.cellB) + 1);
        }
    }

    // Initial propagation
    if (!propagator->propagate(initialState))
    {
        if (config.verbose)
        {
            std::cout << "Initial propagation failed - puzzle is unsolvable\n";
        }
        return solutions;
    }

    // Start search
    if (config.enableParallel && config.threads > 1)
    {
        parallelSearch(initialState);
    }
    else
    {
        search(initialState);
    }

    return solutions;
}

void Solver::search(State &state)
{
    // Check if we should stop
    if (!config.findAll && foundSolution.load())
    {
        return;
    }

    // Validate state
    if (validator->isUnsolvable(state))
    {
        return;
    }

    // Propagate constraints
    if (!propagator->propagate(state))
    {
        return;
    }

    // Select next edge to decide
    int edgeIdx = heuristic->selectNextEdge(state);

    // No more edges - check if we have a solution
    if (edgeIdx == (int)edges.size())
    {
        Solution sol;
        if (extractSolution(state, sol))
        {
            if (validator->isValid(state))
            {
                solutions.push_back(std::move(sol));
                foundSolution.store(true);

                if (config.verbose)
                {
                    std::cout << "Found solution #" << solutions.size() << "\n";
                }

                if (!config.findAll)
                {
                    return;
                }
            }
        }
        return;
    }

    // Determine valid branches
    const Edge &edge = edges[edgeIdx];
    bool canOn = true;
    bool canOff = true;

    int degU = state.getPointDegree(edge.u);
    int degV = state.getPointDegree(edge.v);
    int undU = state.getPointUndecided(edge.u);
    int undV = state.getPointUndecided(edge.v);

    // Forced moves
    if ((degU == 1 && undU == 1) || (degV == 1 && undV == 1))
    {
        canOff = false;
    }
    if (degU >= 2 || degV >= 2)
    {
        canOn = false;
    }

    // Try OFF first
    State offState;
    if (canOff)
    {
        offState = state;
        if (!propagator->applyDecision(offState, edgeIdx, -1))
        {
            canOff = false;
        }
    }

    // Try ON
    State onState;
    if (canOn)
    {
        if (canOff)
        {
            onState = state;
        }
        else
        {
            onState = std::move(state);
        }
        if (!propagator->applyDecision(onState, edgeIdx, 1))
        {
            canOn = false;
        }
    }

    // No valid branches
    if (!canOn && !canOff)
    {
        return;
    }

    // Single branch
    if (canOn && !canOff)
    {
        search(onState);
        return;
    }
    if (!canOn && canOff)
    {
        search(offState);
        return;
    }

    // Both branches valid - explore both
    search(offState);
    if (!config.findAll && foundSolution.load())
    {
        return;
    }
    search(onState);
}

void Solver::parallelSearch(State &initialState)
{
#ifdef USE_TBB
    // Parallel search at top level
    std::vector<State> topStates;
    topStates.push_back(initialState);

    // Generate initial work for parallel execution
    int targetStates = config.threads * 4;
    while ((int)topStates.size() < targetStates)
    {
        std::vector<State> newStates;
        bool expanded = false;

        for (auto &s : topStates)
        {
            int edgeIdx = heuristic->selectNextEdge(s);
            if (edgeIdx == (int)edges.size())
            {
                newStates.push_back(s);
                continue;
            }

            expanded = true;
            State onState = s;
            State offState = s;

            bool canOn = propagator->applyDecision(onState, edgeIdx, 1);
            bool canOff = propagator->applyDecision(offState, edgeIdx, -1);

            if (canOn)
                newStates.push_back(std::move(onState));
            if (canOff)
                newStates.push_back(std::move(offState));
        }

        if (!expanded)
            break;
        topStates = std::move(newStates);
    }

    // Search each state in parallel
    tbb::task_group g;
    for (auto &s : topStates)
    {
        g.run([this, s]()
              {
                  State localState = s;
                  search(localState); });
    }
    g.wait();
#else
    // Fallback to sequential search
    search(initialState);
#endif
}

bool Solver::extractSolution(const State &state, Solution &sol) const
{
    sol.setEdgeState(state.getEdgeStateVector());

    // Extract cycle points
    int numPoints = (grid.getRows() + 1) * (grid.getCols() + 1);
    std::vector<std::vector<int>> adj(numPoints);
    int start = -1;

    for (size_t i = 0; i < edges.size(); ++i)
    {
        if (state.getEdgeState(i) == 1)
        {
            const Edge &e = edges[i];
            adj[e.u].push_back(e.v);
            adj[e.v].push_back(e.u);
            if (start == -1)
                start = e.u;
        }
    }

    if (start == -1)
        return false;

    // Follow cycle
    std::vector<std::pair<int, int>> cyclePoints;
    int prev = -1;
    int curr = start;

    do
    {
        cyclePoints.push_back({curr / (grid.getCols() + 1), curr % (grid.getCols() + 1)});

        int next = -1;
        for (int neighbor : adj[curr])
        {
            if (neighbor != prev)
            {
                next = neighbor;
                break;
            }
        }

        if (next == -1)
            return false;
        prev = curr;
        curr = next;
    } while (curr != start);

    sol.setCyclePoints(cyclePoints);
    return true;
}