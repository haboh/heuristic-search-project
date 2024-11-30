#pragma once

#include "DStarLite.h"

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
        constexpr grid::Grid::Cost inifity_cost = std::numeric_limits<grid::Grid::Cost>::max() / 1000;
        
        grid::Grid::Cost km = 0;
        std::map<grid::GridPoint, grid::Grid::Cost> rhs;
        std::map<grid::GridPoint, grid::Grid::Cost> g;
        std::set<std::pair<std::pair<grid::Grid::Cost, grid::Grid::Cost>, grid::GridPoint>> U;

        auto calculateKey = [&](const grid::GridPoint s){
            return std::make_pair
            (
                std::min(g[s], rhs[s]) + heuristicFunc(start, s) + km, 
                std::min(g[s], rhs[s])
            );
        };

        auto updateVertex = [&](const grid::GridPoint u) {
            if (u != goal) {
                rhs[u] = inifity_cost;
                for (const auto& n : grid.getFreeNeighbours(u))
                {
                    rhs[u] = std::min(rhs[u], g[n] + grid.getCost(u, n));
                }
            }

            {
                while (true) 
                {
                    bool found = false;
                    for (const auto& p : U)
                    {
                        if (p.second == u) {
                            U.erase(p);
                            found = true;
                            break;
                        }
                    }
                    if (not found) break;
                }   
            }

            
            if (g[u] != rhs[u]) U.insert(std::make_pair(calculateKey(u), u));
        };

        auto computeShortesPath = [&]() {
            while (U.begin()->first < calculateKey(start) || rhs[start] != g[start])
            {
                const auto [kold, u] = *U.begin();
                U.erase(U.begin());
                if (grid.occupied(u))
                {
                    continue;
                }
                if (kold < calculateKey(u))
                {
                    U.insert(std::make_pair(calculateKey(u), u));
                }
                else if (g[u] > rhs[u])
                {
                    g[u] = rhs[u];
                    for (const auto s : grid.getFreeNeighbours(u))
                    {
                        updateVertex(s);
                    }
                } else {
                    g[u] = inifity_cost;
                    for (const auto s : grid.getFreeNeighbours(u))
                    {
                        updateVertex(s);
                    }
                    updateVertex(u);
                }
            }
        };

        auto initialize = [&](){
            U.clear();
            for (int i = 0; i < (int)grid.getRows(); ++i)
            {
                for (int j = 0; j < (int)grid.getColumns(); ++j)
                {
                    rhs[grid::GridPoint{i, j}] = inifity_cost;
                    g[grid::GridPoint{i, j}] = inifity_cost;
                }
            }
            rhs[goal] = 0;
            U.insert(std::make_pair(calculateKey(goal), goal));
        };

        result::Path path;
        path.push_back(start);

        grid::GridPoint last = start;
        initialize();
        computeShortesPath();
        while (start != goal)
        {
            if (g[start] >= inifity_cost)
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
                    for (const auto& n : grid.getFreeNeighbours(v))
                    {
                        updateVertex(n);
                    }
                }
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