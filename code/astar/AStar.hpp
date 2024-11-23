#pragma once

#include "AStar.h"

#include <queue>
#include <set>
#include <algorithm>

namespace astar
{
    namespace
    {
        struct Node final
        {
            grid::Grid::Cost f;
            grid::Grid::Cost g;
            grid::GridPoint point;
            Node* parent;

            bool operator<(const Node&) const = default;
        };
    }

    template <typename HeuristicFunc>
    result::Result AStar::findShortestPath
    (
        const grid::Grid& grid,
        grid::GridPoint start,
        grid::GridPoint finish,
        HeuristicFunc heuristicFunc
    )
    {
        std::priority_queue<const Node*> q;
        q.push(new Node{.f = 0, .g = 0, .h = 0, .point = start, .parent = nullptr});
        std::set<grid::GridPoint> expanded;

        Node* finalNode = nullptr;

        while (q.size())
        {
            const Node& best = *q.top();
            q.pop();

            if (expanded.count(best.point))
            {
                continue;
            }

            if (best.point == finish)
            {
                finalNode = best;
                break;
            }
            expanded.insert(best.point);

            for (const auto neighbour : grid.getNeighbours(best.point))
            {
                const grid::Grid::Cost gvalue = best.g + grid.getCost(best.point, neighbour);

                q.push(new Node{
                    .f = gvalue + heuristicFunc(neighbour, finish),
                    .g = gvalue,
                    .point = neighbour,
                    .parent = &best
                });
            }
        }

        result::Result result;

        if (finalNode == nullptr)
        {
            result.pathFound = false;
        } else {
            result.pathFound = true;
            while (finalNode != nullptr)
            {
                result.path.push_back(finalNode->point);
                finalNode = finalNode->parent;
            }
            std::reverse(result.path.begin(), result.path.end());
        }
        result.closedCount = expanded.size();
        result.openCount = q.size();
        result.searchTreeSize = expanded.size() + q.size();
        return result;
    }
}