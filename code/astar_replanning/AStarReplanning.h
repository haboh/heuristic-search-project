#pragma once

#include "GridView.h"

#include "PathSearchResult.h"

namespace unknownterrain
{
    /*
    Idea:
        1. Run A* from current agent's position (with information that we know about map)
        2. A* finds some path, let's move along this path until visit wall cell or goal.
        3. Goto step 1.
    */
    class AStarReplanning final
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

#include "AStarReplanning.hpp"