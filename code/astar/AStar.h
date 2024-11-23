#pragma once

#include "Grid.h"

#include "../PathSearchResult.h"

namespace astar
{
    class AStar final
    {
        template <typename HeuristicFunc>
        result::Result findShortestPath
        (
            const grid::Grid& grid,
            grid::GridPoint start,
            grid::GridPoint finish,
            HeuristicFunc heuristicFunc
        );
    };
}

#include "AStar.hpp"