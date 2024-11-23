#pragma once

#include <vector>
#include <cstddef>
#include <array>
#include <map>

namespace grid
{
    struct GridPoint final
    {
        GridPoint operator+(GridPoint) const;
        GridPoint operator-(GridPoint) const;

        bool operator<(const GridPoint&) const;
        bool operator==(const GridPoint&) const;

        int x;
        int y;
    };

    using Path = std::vector<GridPoint>;

    class Grid final
    {
    public:
        using Cost = int;

        using Field = std::vector<std::vector<bool>>;
        using Costs = std::vector<std::vector<std::map<GridPoint, Cost>>>;

        explicit Grid(Field &&, Costs &&);

        bool occupied(GridPoint) const;

        std::vector<GridPoint> getNeighbours(GridPoint) const;

        Cost getCost(GridPoint start, GridPoint end) const;

        void changeCost(GridPoint start, GridPoint end, Cost);

    private:
        const std::vector<GridPoint> possibleMoves = {
            GridPoint{0, 1},
            GridPoint{0, -1},
            GridPoint{1, 0},
            GridPoint{0, -1}
        };

        bool validGridPoint(GridPoint) const;

        Field field;
        Costs costs;
    };
}
