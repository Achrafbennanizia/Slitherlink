#ifndef SLITHERLINK_SOLVER_GRAPHBUILDER_H
#define SLITHERLINK_SOLVER_GRAPHBUILDER_H

#include "core/Grid.h"
#include <map>
#include <utility>

namespace slitherlink
{

    /**
     * @brief Builds edge index mappings for the puzzle graph (SOLID architecture)
     *
     * Part of planned SOLID refactoring - not currently used.
     */
    class GraphBuilder
    {
    private:
        std::map<std::pair<int, int>, int> horizEdgeIndex;
        std::map<std::pair<int, int>, int> vertEdgeIndex;

    public:
        void buildGraph(const Grid &grid);

        const std::map<std::pair<int, int>, int> &getHorizEdgeIndex() const { return horizEdgeIndex; }
        const std::map<std::pair<int, int>, int> &getVertEdgeIndex() const { return vertEdgeIndex; }
    };

} // namespace slitherlink

#endif // SLITHERLINK_SOLVER_GRAPHBUILDER_H
