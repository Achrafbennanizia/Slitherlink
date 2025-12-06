#ifndef SLITHERLINK_EDGE_H
#define SLITHERLINK_EDGE_H

namespace slitherlink
{

    /**
     * @brief Represents an edge in the puzzle graph
     *
     * Single Responsibility: Edge structure and properties
     */
    struct Edge
    {
        int u;     ///< First endpoint (point index)
        int v;     ///< Second endpoint (point index)
        int cellA; ///< First adjacent cell (-1 if none)
        int cellB; ///< Second adjacent cell (-1 if none)

        Edge(int u, int v, int cellA, int cellB)
            : u(u), v(v), cellA(cellA), cellB(cellB) {}
    };

} // namespace slitherlink

#endif // SLITHERLINK_EDGE_H
