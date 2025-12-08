#ifndef OPTIMIZED_PROPAGATOR_H
#define OPTIMIZED_PROPAGATOR_H

#include "IPropagator.h"
#include "Grid.h"
#include "Edge.h"
#include "State.h"
#include <vector>
#include <cstdint>

namespace slitherlink
{

    class OptimizedPropagator : public IPropagator
    {
    private:
        const Grid &grid;
        const std::vector<Edge> &edges;
        const std::vector<std::vector<int>> &adjacentEdges;
        const std::vector<std::vector<int>> &pointEdges;

        // Phase 2 optimization: uint8_t queues for cache efficiency
        mutable std::vector<uint8_t> cellQueue;
        mutable std::vector<uint8_t> pointQueue;
        mutable std::vector<uint8_t> edgeQueue;

        // Propagation helpers
        bool propagateCell(State &state, int cellIdx) const;
        bool propagatePoint(State &state, int pointIdx) const;
        bool propagateEdge(State &state, int edgeIdx) const;

    public:
        OptimizedPropagator(const Grid &g,
                            const std::vector<Edge> &e,
                            const std::vector<std::vector<int>> &adjEdges,
                            const std::vector<std::vector<int>> &ptEdges)
            : grid(g), edges(e), adjacentEdges(adjEdges), pointEdges(ptEdges)
        {
            // Pre-allocate queues (Phase 1 optimization)
            cellQueue.reserve(g.getRows() * g.getCols());
            pointQueue.reserve((g.getRows() + 1) * (g.getCols() + 1));
            edgeQueue.reserve(e.size());
        }

        bool propagate(State &state) const override;
        bool applyDecision(State &state, int edgeIdx, int value) const override;
    };

} // namespace slitherlink

#endif // OPTIMIZED_PROPAGATOR_H
