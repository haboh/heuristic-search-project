#pragma once

#include "Grid.h"

namespace grid
{
    class GridView final
    {
    public:
        explicit GridView(const Grid&, int radius);
        
        size_t getColumns() const;
        size_t getRows() const;
        size_t getRadius() const;

        bool occupied(GridPoint) const;
        std::vector<GridPoint> getFreeNeighbours(GridPoint) const;
        std::vector<GridPoint> getNeighbours(GridPoint) const;
        std::vector<GridPoint> observe(GridPoint); // returns points that was updated (i.e. new walls)
        Grid::Cost getCost(GridPoint point1, GridPoint point2) const;
        int getRadius() const;

    private:
        const Grid& grid;
        int radius;
        std::vector<std::vector<bool>> viewed;
    };
}
