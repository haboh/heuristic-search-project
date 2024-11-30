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

    Grid::Grid(const Field& f)
        : field(f)
    {
    }

    size_t Grid::getRows() const
    {
        return field.size();
    }

    size_t Grid::getColumns() const
    {
        return field[0].size();
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
            if (validGridPoint(newPoint))
            {
                neighbours.push_back(newPoint);
            }
        }
        return neighbours;
    }

    std::vector<GridPoint> Grid::getFreeNeighbours(const GridPoint point) const
    {
        std::vector<GridPoint> freeNeighbours;
        for (const auto neighbour : getNeighbours(point))
        {
            if (not occupied(neighbour))
            {
                freeNeighbours.push_back(neighbour);
            }
        }
        return freeNeighbours;
    }

    Grid::Cost Grid::getCost(GridPoint point1, GridPoint point2) const
    {
        assert(abs(point1.x - point2.x) + abs(point1.y - point2.y) == 1);

        return 1;
    }
}