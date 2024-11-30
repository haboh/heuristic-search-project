#include "SWSFP.h"

#include <set>

#include <numeric>

namespace unknownterrain
{
    result::PathSearchResult SWSFP::findShortestPath
    (
        grid::GridView gridView,
        grid::GridPoint start,
        const grid::GridPoint goal
    )
    {        
        const grid::Grid::Cost inifity_cost = std::numeric_limits<grid::Grid::Cost>::max() / 1000;

        std::map<grid::GridPoint, grid::Grid::Cost> rhs;
        std::map<grid::GridPoint, grid::Grid::Cost> d;
        std::set<std::pair<grid::Grid::Cost, grid::GridPoint>> inconsistentVertices;

        const auto computeRHS = [&](grid::GridPoint point) {
            if (point == goal)
            {
                rhs[point] = 0;
                return;
            }

            rhs[point] = inifity_cost;
            for (const auto& n : gridView.getFreeNeighbours(point))
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
            }
        };

        const auto updateNeigboursAndVertex = [&](grid::GridPoint point) {
            for (const auto& n : gridView.getFreeNeighbours(point))
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
                if (gridView.occupied(u))
                {
                    continue;
                }
                if (rhs[u] == d[u]) {
                    continue;
                }
                if (rhs[u] < d[u]) {
                    d[u] = rhs[u];
                }
                else {
                    d[u] = inifity_cost;    
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
                rhs[grid::GridPoint{i, j}] = inifity_cost;
                d[grid::GridPoint{i, j}] = inifity_cost;
            }
        }

        rhs[goal] = 0;
        d[goal] = 0;
        updateNeigboursAndVertex(goal);
        computePath();

        if (d[start] == inifity_cost)
        {
            return result::PathSearchResult{
                .pathFound = false,
                .path = {},
                .steps = 0,
                .searchTreeSize = 0,
                .openCount = 0,
                .closedCount = 0
            };
        }

        while (start != goal)
        {
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
            gridView.observe(start);
            computePath();
        }

        return result::PathSearchResult{
            .pathFound = true,
            .path = path,
            .steps = 0,
            .searchTreeSize = 0,
            .openCount = 0,
            .closedCount = 0
        };
    }
}