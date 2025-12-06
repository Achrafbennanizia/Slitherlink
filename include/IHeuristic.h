#ifndef SLITHERLINK_IHEURISTIC_H
#define SLITHERLINK_IHEURISTIC_H

#include "State.h"

namespace slitherlink
{

    /**
     * @brief Interface for edge selection heuristics
     *
     * Interface Segregation Principle: Separate interface for heuristic strategy
     * Open/Closed Principle: Open for extension (new heuristics), closed for modification
     */
    class IHeuristic
    {
    public:
        virtual ~IHeuristic() = default;

        /**
         * @brief Select next edge to decide
         * @param state Current search state
         * @return Index of selected edge, or size() if no edges left
         */
        virtual int selectNextEdge(const State &state) const = 0;
    };

} // namespace slitherlink

#endif // SLITHERLINK_IHEURISTIC_H
