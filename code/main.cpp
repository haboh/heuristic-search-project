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

Field readMapFile(std::string filename) {
    std::ifstream ifs(filename);
    auto readToken = [&ifs](std::string expectedToken) {
        std::string nextToken;
        ifs >> nextToken;
        if (nextToken != expectedToken) {
            std::cout << "Expected: " << expectedToken << ", actual: " << nextToken;
        }
    };
    readToken("type");
    readToken("octile");
    readToken("height");
    int const height = read<int>(ifs);
    readToken("width");
    int const width = read<int>(ifs);
    std::cout << "Read: " << width << " x " << height << std::endl;

    readToken("map");
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


int main()
{
    auto grid = readMapFile("./data/arena.map");
    auto height = grid.size();
    auto width = grid[0].size();
    for (int y = 0; y < (int)height; ++y) {
        for (int x = 0; x < (int)width; ++x) {
            std::cout << (grid[y][x] ? '@' : '.');
        }
        std::cout << std::endl;
    }
    return 0;
}