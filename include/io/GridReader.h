#ifndef SLITHERLINK_IO_GRIDREADER_H
#define SLITHERLINK_IO_GRIDREADER_H

#include "core/Grid.h"
#include <string>

namespace slitherlink
{

    /**
     * @brief Reads puzzle grid from file (SOLID architecture)
     *
     * Part of planned SOLID refactoring - not currently used.
     */
    class GridReader
    {
    public:
        static Grid readFromFile(const std::string &filename);
    };

} // namespace slitherlink

#endif // SLITHERLINK_IO_GRIDREADER_H
