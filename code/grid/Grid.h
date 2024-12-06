#pragma once

#include <vector>
#include <cstddef>
#include <array>
#include <map>

#include <numeric>

namespace grid
{
    struct GridPoint final
    {
        GridPoint operator+(GridPoint) const;
        GridPoint operator-(GridPoint) const;

        auto operator<=>(const GridPoint&) const = default;

        int x;
        int y;
    };

    class Grid final
    {
    public:
        using Cost = long long;
        static constexpr grid::Grid::Cost infinity_cost = std::numeric_limits<grid::Grid::Cost>::max() / 1000;

        using Field = std::vector<std::vector<bool>>; // true means occupied

        explicit Grid(const Field&);

        size_t getRows() const;
        size_t getColumns() const;

        bool occupied(GridPoint) const;
        std::vector<GridPoint> getFreeNeighbours(GridPoint) const;
        std::vector<GridPoint> getNeighbours(GridPoint) const;
        bool validGridPoint(GridPoint) const;
        Cost getCost(GridPoint, GridPoint) const;
        const Field& field; // const means ok to be public

    private:
        const std::vector<GridPoint> possibleMoves = {
            GridPoint{0, 1},
            GridPoint{0, -1},
            GridPoint{1, 0},
            GridPoint{-1, 0}
        };

    };
}
