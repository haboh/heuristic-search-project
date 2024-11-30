#pragma once

#include "GridView.h"

#include "PathSearchResult.h"

namespace unknownterrain
{
    class SWSFP final
    {
    public:
        static result::PathSearchResult findShortestPath
        (
            grid::GridView grid,
            grid::GridPoint start,
            grid::GridPoint goal
        );
        
    };
}