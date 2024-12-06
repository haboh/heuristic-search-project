#include "GridView.h"

#include <cassert>
#include <numeric>

namespace grid
{
    GridView::GridView(const Grid& grid, int radius)
        : grid(grid)
        , radius(radius)
        , viewed(grid.getRows(), std::vector<bool>(grid.getColumns(), false))
    {
        assert(radius > 0);
    }

    size_t GridView::getRows() const
    {
        return grid.getRows();
    }

    size_t GridView::getColumns() const
    {
        return grid.getColumns();
    }

    bool GridView::occupied(const GridPoint point) const
    {
        assert(grid.validGridPoint(point));
        if (not viewed[point.x][point.y])
        {
            return false;
        }
        return grid.occupied(point);
    }

    Grid::Cost GridView::getCost(GridPoint point1, GridPoint point2) const
    {
        if (occupied(point1) || occupied(point2))
        {
            return Grid::infinity_cost;
        }

        return grid.getCost(point1, point2);
    }
    

    std::vector<GridPoint> GridView::getNeighbours(const GridPoint point) const
    {
        return grid.getNeighbours(point);
    }

    std::vector<GridPoint> GridView::getFreeNeighbours(const GridPoint point) const
    {
        std::vector<GridPoint> freeNeighbours;
        for (const auto neighbour : grid.getNeighbours(point))
        {
            if (not occupied(neighbour))
            {
                freeNeighbours.push_back(neighbour);
            }
        }
        return freeNeighbours;
    }

    int GridView::getRadius() const {
        return radius;
    }

    std::vector<GridPoint> GridView::observe(const GridPoint point)
    {
        std::vector<GridPoint> updatedPoints;

        for (int i = point.x - radius + 1; i <= point.x + radius - 1; i++)
        {
            for (int j = point.y - radius + 1; j <= point.y + radius - 1; j++)
            {
                if (grid.validGridPoint(GridPoint{i, j}))
                {
                    if (not viewed[i][j] && grid.occupied(GridPoint{i, j}))
                    {
                        updatedPoints.push_back(GridPoint{i, j});
                    }
                    viewed[i][j] = true;
                }
            }
        }

        return updatedPoints;
    }
}