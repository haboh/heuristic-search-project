#pragma once

#include <string>
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
            HeuristicFunc heuristicFunc,
            std::string output_name,
            bool needAnimation = false
        );

        template <typename HeuristicFunc>
        static result::PathSearchResult findShortestPathWithAnimation
        (
            grid::GridView grid,
            grid::GridPoint start,
            grid::GridPoint goal,
            HeuristicFunc heuristicFunc,
            std::string output_name
        );
    };
}

#include "DStarLite.hpp"