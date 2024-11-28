#include <cassert>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "astar/AStar.h"
#include "grid/Grid.h"

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
    std::cout << "Read: " << width << " x " << height << std::endl;

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
    int start_x;
    int start_y;
    int goal_x;
    int goal_y;
    double correctAnswer;
};

std::vector<Task> readTasksFromFile(std::string filename) {
    std::ifstream ifs {filename };
    // for (int i = 0; i < 10; ++i) {
    //     auto r = read<std::string>(ifs);
    //     std::cout << r << std::endl;
    // }
    readToken(ifs, "version");
    readToken(ifs, "1");
    std::vector<Task> result;
    while (true) {
        if (ifs.eof() || ifs.fail() ||ifs.bad())
            return result;
        result.push_back(Task {
            read<int>(ifs),
            read<std::string>(ifs),
            read<int>(ifs),
            read<int>(ifs),
            read<int>(ifs),
            read<int>(ifs),
            read<int>(ifs),
            read<int>(ifs),
            read<double>(ifs),
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

void displayTasks(std::string filename) {
    auto tasks = readTasksFromFile(filename);
    for (auto task: tasks) {
        std::cout << "task bucket=" << task.bucket 
            << " from (" <<task.start_x << ", " << task.start_y << ")"
            << " to (" << task.goal_x << ", " << task.goal_y << ")" << std::endl;
    }
    std::cout << "In total: " << tasks.size() << " entries\n";
}

int main()
{
    // displayMap("./data/arena.map");
    displayTasks("./data/arena.map.scene");
    return 0;
}