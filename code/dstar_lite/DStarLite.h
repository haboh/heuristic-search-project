#pragma once

#include "GridView.h"

#include "PathSearchResult.h"

namespace unknownterrain
{
    class DStarLite final
    {
    public:
        template <typename HeuristicFunc>
        static result::PathSearchResult findShortestPath
        (
            grid::GridView grid,
            grid::GridPoint start,
            grid::GridPoint goal,
            HeuristicFunc heuristicFunc
        );
    };
}

#include "DStarLite.hpp"