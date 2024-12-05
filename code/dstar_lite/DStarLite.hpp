#pragma once

#include "DStarLite.h"

#include <fstream>
#include <numeric>
#include <set>

namespace unknownterrain
{
    template <typename HeuristicFunc>
    result::PathSearchResult DStarLite::findShortestPath
    (
        grid::GridView grid,
        grid::GridPoint start,
        const grid::GridPoint goal,
        HeuristicFunc heuristicFunc
    )
    {
        grid::Grid::Cost km;
        std::map<grid::GridPoint, grid::Grid::Cost> rhs;
        std::map<grid::GridPoint, grid::Grid::Cost> g;
        std::set<std::pair<std::pair<grid::Grid::Cost, grid::Grid::Cost>, grid::GridPoint>> U;
        std::map<grid::GridPoint, std::set<std::pair<grid::Grid::Cost, grid::Grid::Cost>>> UInvert;

        auto calculateKey = [&](const grid::GridPoint s){
            return std::make_pair
            (
                std::min(g[s], rhs[s]) + heuristicFunc(start, s) + km,
                std::min(g[s], rhs[s])
            );
        };

        auto updateVertex = [&](const grid::GridPoint u) {
            if (u != goal) {
                rhs[u] = grid::Grid::infinity_cost;
                for (const auto& n : grid.getNeighbours(u))
                {
                    rhs[u] = std::min(rhs[u], g[n] + grid.getCost(u, n));
                }
            }

            {
                for (const auto& p : UInvert[u])
                {
                    U.erase(std::make_pair(p, u));
                }
                UInvert[u].clear();
            }


            if (g[u] != rhs[u]) {
                U.insert(std::make_pair(calculateKey(u), u));
                UInvert[u].insert(calculateKey(u));
            }
        };

        auto computeShortestPath = [&]() {
            while (U.begin()->first < calculateKey(start) || rhs[start] != g[start])
            {
                const auto [kold, u] = *U.begin();
                U.erase(U.begin());
                UInvert[u].erase(kold);
                if (kold < calculateKey(u))
                {
                    U.insert(std::make_pair(calculateKey(u), u));
                    UInvert[u].insert(calculateKey(u));
                }
                else if (g[u] > rhs[u])
                {
                    g[u] = rhs[u];
                    for (const auto s : grid.getNeighbours(u))
                    {
                        updateVertex(s);
                    }
                } else {
                    g[u] = grid::Grid::infinity_cost;
                    updateVertex(u);
                    for (const auto s : grid.getNeighbours(u))
                    {
                        updateVertex(s);
                    }
                }
            }
        };

        auto initialize = [&](){
            U.clear();
            km = 0;
            for (int i = 0; i < (int)grid.getRows(); ++i)
            {
                for (int j = 0; j < (int)grid.getColumns(); ++j)
                {
                    rhs[grid::GridPoint{i, j}] = grid::Grid::infinity_cost;
                    g[grid::GridPoint{i, j}] = grid::Grid::infinity_cost;
                }
            }
            rhs[goal] = 0;
            U.insert(std::make_pair(calculateKey(goal), goal));
            UInvert[goal].insert(calculateKey(goal));
        };


        result::Path path;
        path.push_back(start);

        grid.observe(start);

        grid::GridPoint last = start;
        initialize();
        computeShortestPath();
        while (start != goal)
        {
            if (g[start] >= grid::Grid::infinity_cost)
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

            const auto& neigbours = grid.getFreeNeighbours(start);
            assert(neigbours.size() > 0);
            grid::GridPoint newStart = neigbours.at(0);
            for (const auto s : neigbours)
            {
                if (g[s] + grid.getCost(start, s) < g[newStart] + grid.getCost(start, newStart))
                {
                    newStart = s;
                }
            }
            start = newStart;
            path.push_back(start);
            const auto changed = grid.observe(start);

            if (changed.size())
            {
                km += heuristicFunc(last, start);
                last = start;
                for (const auto& v : changed)
                {
                    updateVertex(v);
                    for (const auto& n : grid.getNeighbours(v))
                    {
                        updateVertex(n);
                    }
                }
                computeShortestPath();
            }
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