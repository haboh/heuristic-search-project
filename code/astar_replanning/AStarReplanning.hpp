#pragma once

#include "AStarReplanning.h"

#include "AStar.h"

namespace unknownterrain
{
    template <typename HeuristicFunc>
    result::PathSearchResult AStarReplanning::findShortestPath
    (
        grid::GridView gridView,
        const grid::GridPoint start,
        const grid::GridPoint goal,
        HeuristicFunc heuristicFunc
    )
    {        
        result::Path path = {start};
        gridView.observe(start);

        size_t vertexAccesses = 0;
        size_t setAccesses = 0;

        size_t steps = 0;
        size_t searchTreeSize = 0;
        size_t openCount = 0;
        size_t closedCount = 0;

        grid::GridPoint currentPoint = start;
        while (currentPoint != goal)
        {
            const auto res = astar::AStar::findShortestPath(gridView, currentPoint, goal, heuristicFunc);
            steps += res.steps;
            searchTreeSize += res.searchTreeSize;
            openCount += res.openCount;
            closedCount += res.closedCount;
            vertexAccesses += res.vertexAccesses;
            setAccesses += res.priorityQueueAccesses;

            if (not res.pathFound)
            {
                return result::PathSearchResult{
                    .pathFound = false,
                    .path = {},
                    .steps = steps,
                    .searchTreeSize = searchTreeSize,
                    .openCount = openCount,
                    .closedCount = closedCount
                };
            }

            assert(res.path.front() == currentPoint);
            assert(res.path.size() > 1);

            size_t i = 1;
            while (i < res.path.size() && not gridView.occupied(res.path[i]))
            {
                gridView.observe(res.path[i]);
                currentPoint = res.path[i];
                path.push_back(currentPoint);
                i++;
            }
        }

        return result::PathSearchResult{
            .pathFound = true,
            .path = path,
            .steps = steps,
            .searchTreeSize = searchTreeSize,
            .openCount = openCount,
            .closedCount = closedCount,
            .vertexAccesses = vertexAccesses,
            .priorityQueueAccesses = setAccesses,
        };
    }
}