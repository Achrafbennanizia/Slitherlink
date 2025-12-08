#ifndef STANDARD_VALIDATOR_H
#define STANDARD_VALIDATOR_H

#include "IValidator.h"
#include "Grid.h"
#include "Edge.h"
#include "State.h"
#include <vector>
#include <cstdint>

namespace slitherlink
{

    class StandardValidator : public IValidator
    {
    private:
        const Grid &grid;
        const std::vector<Edge> &edges;
        const std::vector<std::vector<int>> &adjacentEdges;
        const std::vector<std::vector<int>> &pointEdges;

        // Quick validation helpers
        bool quickValidityCheck(const State &state) const;
        bool isDefinitelyUnsolvable(const State &state) const;
        bool hasCycle(const State &state) const;
        bool checkCellConstraints(const State &state) const;

    public:
        StandardValidator(const Grid &g,
                          const std::vector<Edge> &e,
                          const std::vector<std::vector<int>> &adjEdges,
                          const std::vector<std::vector<int>> &ptEdges)
            : grid(g), edges(e), adjacentEdges(adjEdges), pointEdges(ptEdges) {}

        bool isValid(const State &state) const override;
        bool isUnsolvable(const State &state) const override;
    };

} // namespace slitherlink

#endif // STANDARD_VALIDATOR_H
