#include "Grid.h"

#include <cassert>

namespace grid
{
    GridPoint GridPoint::operator+(GridPoint other) const
    {
        return GridPoint{x + other.x, y + other.y};
    }

    GridPoint GridPoint::operator-(GridPoint other) const
    {
        return GridPoint{x - other.x, y - other.y};
    }

    bool GridPoint::operator<(const GridPoint& other) const
    {
        return std::make_pair(x, y) < std::make_pair(other.x, other.y);
    }

    Grid::Grid(Field &&f, Costs &&c)
        : field(std::move(f))
        , costs(std::move(c))
    {
    }

    bool Grid::validGridPoint(const GridPoint point) const
    {
        return static_cast<int>(field.size()) > point.x && static_cast<int>(field[0].size()) > point.y && point.x >= 0 && point.y >= 0;
    }

    bool Grid::occupied(const GridPoint point) const
    {
        assert(validGridPoint(point));
        return field[point.x][point.y];
    }

    std::vector<GridPoint> Grid::getNeighbours(const GridPoint point) const
    {
        std::vector<GridPoint> neighbours;
        for (const auto possibleMove : possibleMoves)
        {
            const GridPoint newPoint = point + possibleMove;
            if (validGridPoint(newPoint) && not occupied(newPoint))
            {
                neighbours.push_back(newPoint);
            }
        }
        return neighbours;
    }

    Grid::Cost Grid::getCost(GridPoint p1, GridPoint p2) const
    {
        return costs[p1.x][p1.y].at(p2 - p1);
    }

    void Grid::changeCost(GridPoint p1, GridPoint p2, Cost c)
    {
        costs[p1.x][p1.y].at(p2 - p1) = c;
    }
}