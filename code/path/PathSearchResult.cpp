#include "PathSearchResult.h"

#include <optional>

namespace result
{
    bool validatePath(const Path& path, const grid::Grid& grid)
    {
        std::optional<grid::GridPoint> previousPoint;
        for (const auto point : path)
        {
            if (not grid.validGridPoint(point)) return false;
            const auto& neighbours = grid.getFreeNeighbours(point);
            if (previousPoint && std::find(neighbours.begin(), neighbours.end(), point) == neighbours.end())
            {
                return false;
            }
            previousPoint = point;
        }
        return true;
    }
}