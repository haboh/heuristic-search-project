#include <cassert>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "AStarReplanning.h"
#include "Grid.h"
#include "GridView.h"
#include "SWSFP.h"
#include "DStarLite.h"
#include "PathSearchResult.h"

template <typename T>
T read(std::istream& is) {
    T t;
    is >> t;
    return t;
}

using Field = std::vector<std::vector<bool>>; // true means occupied

void readToken(std::istream& is, std::string expectedToken) {
    std::string nextToken;
    is >> nextToken;
    if (nextToken != expectedToken) {
        std::cout << "Expected: " << expectedToken << ", actual: {" << nextToken << "}\n";
    }
};

Field readMapFile(std::string filename) {
    std::ifstream ifs{filename};
    readToken(ifs, "type");
    readToken(ifs, "octile");
    readToken(ifs, "height");
    int const height = read<int>(ifs);
    readToken(ifs, "width");
    int const width = read<int>(ifs);

    readToken(ifs, "map");
    std::vector<std::vector<bool>> grid(height, std::vector<bool>(width, false));
    for (int y = 0; y < height; ++y) {
        std::string const line = read<std::string>(ifs);
        assert((int)line.length() == width);
        for (int x = 0; x < width; ++x) {
            char c = line[x];
            if (c == '.' || c == 'G') {
                grid[y][x] = false;
            } else if (c == '@' || c == 'T' || c == 'O') {
                grid[y][x] = true;
            } else {
                assert(false && "unsupported symbol");
            }
        }
    }
    return grid;
}

struct Task {
    int bucket;
    std::string name;
    int map_width;
    int height;
    grid::GridPoint start;
    grid::GridPoint goal;
    double correctAnswer;
};

std::vector<Task> readTasksFromFile(std::string filename) {
    std::ifstream ifs {filename };
    readToken(ifs, "version");
    readToken(ifs, "1");
    std::vector<Task> result;
    while (true) {
        if (ifs.eof() || ifs.fail() ||ifs.bad())
            return result;
        int bucket = read<int>(ifs);
        std::string name =        read<std::string>(ifs);
        int map_width =           read<int>(ifs);
        int height =              read<int>(ifs);
        int start_x =             read<int>(ifs);
        int start_y =             read<int>(ifs);
        int goal_x =              read<int>(ifs);
        int goal_y =              read<int>(ifs);
        double correctAnswer =    read<double>(ifs);

        result.push_back(Task {
            bucket,
            name,
            map_width,
            height,
            grid::GridPoint{start_x, start_y},
            grid::GridPoint{goal_x, goal_y},
            correctAnswer
        });
    }
}

void displayMap(std::string filename) {
    auto grid = readMapFile(filename);
    auto height = grid.size();
    auto width = grid[0].size();
    for (int y = 0; y < (int)height; ++y) {
        for (int x = 0; x < (int)width; ++x) {
            std::cout << (grid[y][x] ? '@' : '.');
        }
        std::cout << std::endl;
    }
}


void displayPath(const Field& grid, const result::Path& path)
{
    auto height = grid.size();
    auto width = grid[0].size();
    for (int y = 0; y < (int)height; ++y) {
        for (int x = 0; x < (int)width; ++x) {
            if (std::find(path.begin(), path.end(), grid::GridPoint{y, x}) != path.end()) {
                std::cout << '$';
            }
            else {
                std::cout << (grid[y][x] ? '@' : '.');
            }
        }
        std::cout << std::endl;
    }
}


void displayTasks(std::string filename) {
    auto tasks = readTasksFromFile(filename);
    for (auto task: tasks) {
        std::cout << "task bucket=" << task.bucket 
            << " from (" <<task.start.x << ", " << task.start.y << ")"
            << " to (" << task.goal.x << ", " << task.goal.y << ")" << std::endl;
    }
    std::cout << "In total: " << tasks.size() << " entries\n";
}


grid::Grid::Cost manhattan(grid::GridPoint point1, grid::GridPoint point2)
{
    return std::abs(point1.x - point2.x) + abs(point2.y - point1.y);
}

int main()
{
    const auto map = readMapFile("./data/arena.map");
    const auto tasks = readTasksFromFile("./data/arena.map.scene");
    grid::Grid grid(map);
    
    const auto res = unknownterrain::AStarReplanning::findShortestPath(grid::GridView(grid, 2), grid::GridPoint{10, 10}, grid::GridPoint{20, 20}, manhattan);
    // const auto res = unknownterrain::SWSFP::findShortestPath(grid::GridView(grid, 2), grid::GridPoint{10, 10}, grid::GridPoint{40, 35});
    // const auto res = unknownterrain::DStarLite::findShortestPath(grid::GridView(grid, 2), grid::GridPoint{10, 10}, grid::GridPoint{30, 30}, manhattan);

    assert(res.pathFound);
    result::validatePath(res.path, grid);
    displayPath(map, res.path);
    return 0;
}