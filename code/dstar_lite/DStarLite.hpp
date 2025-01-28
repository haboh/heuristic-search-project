#pragma once

#include "DStarLite.h"
#include "Grid.h"
#include "json.hpp"
#include <fstream>
#include <numeric>
#include <set>
#include <string>

using json = nlohmann::json;

namespace grid {
    void to_json(json& j, const grid::GridPoint& p) 
    {
        j = json{{"x", p.y}, {"y", p.x}};
    }
    }

constexpr bool SUPPORT_ANIMATION = true;

namespace unknownterrain
{
    template <typename HeuristicFunc>
    result::PathSearchResult DStarLite::findShortestPath
    (
        grid::GridView gridView,
        grid::GridPoint start,
        const grid::GridPoint goal,
        HeuristicFunc heuristicFunc,
        std::string output_name,
        bool needAnimation
    )
    {
        grid::Grid::Cost km;
        std::map<grid::GridPoint, grid::Grid::Cost> rhs;
        std::map<grid::GridPoint, grid::Grid::Cost> g;
        std::set<std::pair<std::pair<grid::Grid::Cost, grid::Grid::Cost>, grid::GridPoint>> U;
        std::map<grid::GridPoint, std::set<std::pair<grid::Grid::Cost, grid::Grid::Cost>>> UInvert;
        size_t vertexAccesses = 0;
        size_t setAccesses = 0;

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
                for (const auto& n : gridView.getNeighbours(u))
                {
                    rhs[u] = std::min(rhs[u], g[n] + gridView.getCost(u, n));
                    vertexAccesses += 1;
                }
            }

            {
                for (const auto& p : UInvert[u])
                {
                    U.erase(std::make_pair(p, u));
                    setAccesses += 1;
                }
                UInvert[u].clear();
            }


            if (g[u] != rhs[u]) {
                U.insert(std::make_pair(calculateKey(u), u));
                setAccesses += 1;
                UInvert[u].insert(calculateKey(u));
            }
        };
        std::vector<grid::GridPoint> latelyExpanded;
        auto computeShortestPath = [&]() {
            latelyExpanded.clear();
            while (U.begin()->first < calculateKey(start) || rhs[start] != g[start])
            {
                const auto [kold, u] = *U.begin();
                U.erase(U.begin());
                UInvert[u].erase(kold);
                latelyExpanded.push_back(u);
                vertexAccesses += 1;
                setAccesses += 1;
                if (kold < calculateKey(u))
                {
                    U.insert(std::make_pair(calculateKey(u), u));
                    setAccesses += 1;
                    UInvert[u].insert(calculateKey(u));
                }
                else if (g[u] > rhs[u])
                {
                    g[u] = rhs[u];
                    for (const auto s : gridView.getNeighbours(u))
                    {
                        updateVertex(s);
                    }
                } else {
                    g[u] = grid::Grid::infinity_cost;
                    updateVertex(u);
                    for (const auto s : gridView.getNeighbours(u))
                    {
                        updateVertex(s);
                    }
                }
            }
        };

        auto initialize = [&](){
            U.clear();
            km = 0;
            for (int i = 0; i < (int)gridView.getRows(); ++i)
            {
                for (int j = 0; j < (int)gridView.getColumns(); ++j)
                {
                    rhs[grid::GridPoint{i, j}] = grid::Grid::infinity_cost;
                    g[grid::GridPoint{i, j}] = grid::Grid::infinity_cost;
                }
            }
            rhs[goal] = 0;
            U.insert(std::make_pair(calculateKey(goal), goal));
            setAccesses += 1;
            UInvert[goal].insert(calculateKey(goal));
        };


        result::Path path;
        path.push_back(start);

        gridView.observe(start);

        grid::GridPoint last = start;
        initialize();
        computeShortestPath();
        std::vector<grid::GridPoint> allObstacles;
        std::vector<grid::GridPoint> firstSeen;
        for (size_t y = 0; y < gridView.getRows(); ++y) 
        {
            for (size_t x = 0; x < gridView.getColumns(); ++x) 
            {
                auto point = grid::GridPoint(y, x);
                if (gridView.grid.occupied(point)) 
                {
                    allObstacles.push_back(point);
                }
                if (gridView.occupied(point)) {
                    firstSeen.push_back(point);
                }
            }
        }
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

            const auto& neigbours = gridView.getFreeNeighbours(start);
            assert(neigbours.size() > 0);
            grid::GridPoint newStart = neigbours.at(0);
            for (const auto s : neigbours)
            {
                if (g[s] + gridView.getCost(start, s) < g[newStart] + gridView.getCost(start, newStart))
                {
                    newStart = s;
                }
            }
            start = newStart;
            path.push_back(start);
            const auto changed = gridView.observe(start);
            latelyExpanded.clear();
            if (changed.size())
            {
                km += heuristicFunc(last, start);
                last = start;
                for (const auto& v : changed)
                {
                    updateVertex(v);
                    for (const auto& n : gridView.getNeighbours(v))
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
            .closedCount = 0,
            .vertexAccesses = vertexAccesses,
            .priorityQueueAccesses = setAccesses,
        };
    }
    
    template <typename HeuristicFunc>
    result::PathSearchResult DStarLite::findShortestPathWithAnimation
    (
        grid::GridView grid,
        grid::GridPoint start,
        const grid::GridPoint goal,
        HeuristicFunc heuristicFunc,
        std::string output_name
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
        std::vector<grid::GridPoint> latelyExpanded;
        auto computeShortestPath = [&]() {
            latelyExpanded.clear();
            while (U.begin()->first < calculateKey(start) || rhs[start] != g[start])
            {
                const auto [kold, u] = *U.begin();
                U.erase(U.begin());
                UInvert[u].erase(kold);
                latelyExpanded.push_back(u);
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

        auto tracePath = [&](){
            std::vector<grid::GridPoint> tracedPath;
            auto current = start;
            while(current != goal) {
                const auto& neigbours = grid.getFreeNeighbours(current);
                assert(neigbours.size() > 0);
                grid::GridPoint newStart = neigbours.at(0);
                for (const auto s : neigbours)
                {
                    if (g[s] + grid.getCost(current, s) < g[newStart] + grid.getCost(current, newStart))
                    {
                        newStart = s;
                    }
                }
                tracedPath.push_back(newStart);
                current = newStart;
            };
            return tracedPath;
        };

        result::Path path;
        path.push_back(start);

        grid.observe(start);

        grid::GridPoint last = start;
        initialize();
        computeShortestPath();
        json result = json::object();
        result["columns"] = grid.getColumns();
        result["rows"] = grid.getRows();
        result["radius"] = grid.getRadius();
        result["start"] = start;
        result["goal"] = goal;
        std::vector<grid::GridPoint> allObstacles;
        std::vector<grid::GridPoint> firstSeen;
        for (size_t y = 0; y < grid.getRows(); ++y) 
        {
            for (size_t x = 0; x < grid.getColumns(); ++x) 
            {
                auto point = grid::GridPoint(y, x);
                if (grid.grid.occupied(point)) 
                {
                    allObstacles.push_back(point);
                }
                if (grid.occupied(point)) {
                    firstSeen.push_back(point);
                }
            }
        }
        result["all_obstacles"] = allObstacles;

        json steps = json::array();

        json firstStep = json::object();
        firstStep["pos"] = start;
        firstStep["new_obstacles"] = firstSeen;
        firstStep["expanded_now"] = latelyExpanded;
        firstStep["predicted_path"] = tracePath();
        steps.push_back(firstStep);
        while (start != goal)
        {
            json step = json::object();
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
            step["pos"] = newStart;
            path.push_back(start);
            const auto changed = grid.observe(start);
            json chng = json::array();
            latelyExpanded.clear();
            if (changed.size())
            {
                km += heuristicFunc(last, start);
                last = start;
                for (const auto& v : changed)
                {
                    chng.push_back({ {"x", v.y}, {"y", v.x} });
                    updateVertex(v);
                    for (const auto& n : grid.getNeighbours(v))
                    {
                        updateVertex(n);
                    }
                }
                computeShortestPath();
            }
            step["expanded_now"] = latelyExpanded;
            step["predicted_path"] = tracePath();
            step["new_obstacles"] = chng;
            steps.push_back(step);
        }
        result["steps"] = steps;
        result["path"] = path;
        std::ofstream ofjson(output_name);
        ofjson << result.dump(4);

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