#pragma once

#include "grid/Grid.h"

namespace result
{
    struct Result
    {
        bool pathFound;
        grid::Path path;
        size_t steps;
        size_t searchTreeSize;
        size_t openCount;
        size_t closedCount;
    };
}