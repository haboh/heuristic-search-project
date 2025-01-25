#include "SWSFP.h"

#include <set>

#include <numeric>
#include <iostream>

namespace unknownterrain
{
    result::PathSearchResult SWSFP::findShortestPath
    (
        grid::GridView gridView,
        grid::GridPoint start,
        const grid::GridPoint goal
    )
    {
        std::map<grid::GridPoint, grid::Grid::Cost> rhs;
        std::map<grid::GridPoint, grid::Grid::Cost> d;
        std::set<std::pair<grid::Grid::Cost, grid::GridPoint>> inconsistentVertices;
        size_t vertexAccesses = 0;
        size_t setAccesses = 0;

        const auto computeRHS = [&](grid::GridPoint point) {
            if (point == goal)
            {
                rhs[point] = 0;
                return;
            }

            rhs[point] = grid::Grid::infinity_cost;
            for (const auto& n : gridView.getNeighbours(point))
            {
                rhs[point] = std::min(d[n] + gridView.getCost(point, n), rhs[point]);
            }
        };

        const auto computeKey = [&](grid::GridPoint point) {
            return std::min(d[point], rhs[point]);
        };

        const auto checkInconsistency = [&](grid::GridPoint point) {
            if (rhs[point] != d[point])
            {
                inconsistentVertices.insert({computeKey(point), point});
                setAccesses += 1;
            }
        };

        const auto updateNeigboursAndVertex = [&](grid::GridPoint point) {
            for (const auto& n : gridView.getNeighbours(point))
            {
                computeRHS(n);
                checkInconsistency(n);
            }
            computeRHS(point);
            checkInconsistency(point);
        };

        const auto computePath = [&]() {
            while (inconsistentVertices.size())
            {
                const auto [key, u] = *inconsistentVertices.begin();
                inconsistentVertices.erase(inconsistentVertices.begin());
                setAccesses += 1;
                vertexAccesses += 1;
                if (rhs[u] == d[u]) {
                    continue;
                }
                if (rhs[u] < d[u]) {
                    d[u] = rhs[u];
                }
                else {
                    d[u] = grid::Grid::infinity_cost;
                }
                updateNeigboursAndVertex(u);
            }
        };

        result::Path path = {start};
        gridView.observe(start);

        for (int i = 0; i < (int)gridView.getRows(); ++i)
        {
            for (int j = 0; j < (int)gridView.getColumns(); ++j)
            {
                rhs[grid::GridPoint{i, j}] = grid::Grid::infinity_cost;
                d[grid::GridPoint{i, j}] = grid::Grid::infinity_cost;
            }
        }

        rhs[goal] = 0;
        inconsistentVertices.insert({computeKey(goal), goal});
        setAccesses += 1;
        computePath();


        while (start != goal)
        {
            if (d[start] >= grid::Grid::infinity_cost)
            {
                return result::PathSearchResult{
                    .pathFound = false,
                    .path = {},
                    .steps = 0,
                    .searchTreeSize = 0,
                    .openCount = 0,
                    .closedCount = 0,
                    .vertexAccesses = vertexAccesses,
                    .priorityQueueAccesses = setAccesses,
                };
            }

            const auto& neighbours = gridView.getFreeNeighbours(start);
            start = neighbours.at(0);
            for (const auto& n : neighbours)
            {
                if (d[n] < d[start])
                {
                    start = n;
                }
            }
            path.push_back(start);
            for (const auto& n : gridView.observe(start))
            {
                updateNeigboursAndVertex(n);
            }
            updateNeigboursAndVertex(start);
            computePath();
        }

        return result::PathSearchResult{
            .pathFound = true,
            .path = path,
            .steps = 0,
            .searchTreeSize = 0,
            .openCount = 0,
            .closedCount = 0,
            .vertexAccesses = vertexAccesses,
            .priorityQueueAccesses = setAccesses,
        };
    }
}