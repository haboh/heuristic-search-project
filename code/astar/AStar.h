#pragma once

#include "GridView.h"

#include "PathSearchResult.h"

namespace astar
{
    class AStar final
    {
    public:
        template <typename HeuristicFunc>
        static result::PathSearchResult findShortestPath
        (
            const grid::GridView& grid,
            grid::GridPoint start,
            grid::GridPoint goal,
            HeuristicFunc heuristicFunc
        );
    };
}

#include "AStar.hpp"