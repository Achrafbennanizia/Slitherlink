#ifndef SLITHERLINK_IVALIDATOR_H
#define SLITHERLINK_IVALIDATOR_H

#include "State.h"

namespace slitherlink
{

    /**
     * @brief Interface for state validation strategies
     *
     * Interface Segregation Principle: Separate validation concerns
     */
    class IValidator
    {
    public:
        virtual ~IValidator() = default;

        /**
         * @brief Quick validity check for state
         * @param state State to validate
         * @return true if state is valid, false otherwise
         */
        virtual bool isValid(const State &state) const = 0;

        /**
         * @brief Check if state is definitely unsolvable
         * @param state State to check
         * @return true if definitely unsolvable, false otherwise
         */
        virtual bool isUnsolvable(const State &state) const = 0;
    };

} // namespace slitherlink

#endif // SLITHERLINK_IVALIDATOR_H
