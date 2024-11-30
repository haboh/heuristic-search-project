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
    };

    bool validatePath(const Path&, const grid::Grid&);
}