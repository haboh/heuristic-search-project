#pragma once

#include "Grid.h"

namespace result
{
    using Path = std::vector<grid::GridPoint>;

    struct PathSearchResult final
    {
        bool pathFound;
        Path path;
        size_t steps;
        size_t searchTreeSize;
        size_t openCount;
        size_t closedCount;
        size_t vertexAccesses = -1;
        size_t priorityQueueAccesses = -1;
    };

    bool validatePath(const Path&, const grid::Grid&);
}