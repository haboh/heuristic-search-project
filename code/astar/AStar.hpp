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
            const Node* parent;

            auto operator<=>(const Node&) const = default;
        };

        struct compareNodes {
            bool operator() (const Node* node1, const Node* node2) const {
                return (*node1) > (*node2);
            }
        };
    }

    template <typename HeuristicFunc>
    result::PathSearchResult AStar::findShortestPath
    (
        const grid::GridView& grid,
        grid::GridPoint start,
        grid::GridPoint goal,
        HeuristicFunc heuristicFunc
    )
    {
        std::priority_queue<const Node*, std::vector<const Node*>, compareNodes> q;
        q.push(new Node{.f = 0, .g = 0, .point = start, .parent = nullptr});
        std::set<grid::GridPoint> expanded;

        const Node* finalNode = nullptr;

        while (q.size())
        {
            const Node& best = *q.top();
            q.pop();

            if (expanded.count(best.point))
            {
                continue;
            }

            if (best.point == goal)
            {
                finalNode = &best;
                break;
            }
            expanded.insert(best.point);

            for (const auto neighbour : grid.getFreeNeighbours(best.point))
            {
                const grid::Grid::Cost gvalue = best.g + grid.getCost(best.point, neighbour);

                q.push(new Node{
                    .f = gvalue + heuristicFunc(neighbour, goal),
                    .g = gvalue,
                    .point = neighbour,
                    .parent = &best
                });
            }
        }

        result::PathSearchResult result;

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