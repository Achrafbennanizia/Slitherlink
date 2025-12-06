#include "State.h"

namespace slitherlink
{

    void State::initialize(size_t edgeCount, size_t pointCount, size_t cellCount)
    {
        edgeState.reserve(edgeCount);
        edgeState.assign(edgeCount, 0);

        pointDegree.resize(pointCount, 0);
        pointUndecided.resize(pointCount, 0);
        cellEdgeCount.resize(cellCount, 0);
        cellUndecided.resize(cellCount, 0);
    }

} // namespace slitherlink
