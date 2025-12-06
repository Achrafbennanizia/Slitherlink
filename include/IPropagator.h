#ifndef SLITHERLINK_IPROPAGATOR_H
#define SLITHERLINK_IPROPAGATOR_H

#include "State.h"

namespace slitherlink
{

    /**
     * @brief Interface for constraint propagation strategies
     *
     * Interface Segregation Principle: Separate propagation logic
     * Strategy Pattern: Different propagation algorithms
     */
    class IPropagator
    {
    public:
        virtual ~IPropagator() = default;

        /**
         * @brief Propagate constraints from current state
         * @param state State to propagate (modified in place)
         * @return true if propagation succeeded, false if contradiction found
         */
        virtual bool propagate(State &state) const = 0;

        /**
         * @brief Apply a decision to the state
         * @param state State to modify
         * @param edgeIdx Edge index
         * @param value Decision value (1=ON, -1=OFF)
         * @return true if decision is valid, false otherwise
         */
        virtual bool applyDecision(State &state, int edgeIdx, int value) const = 0;
    };

} // namespace slitherlink

#endif // SLITHERLINK_IPROPAGATOR_H
