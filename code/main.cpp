#include <argparse.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AStarReplanning.h"
#include "DStarLite.h"
#include "Grid.h"
#include "GridView.h"
#include "PathSearchResult.h"
#include "SWSFP.h"

template <typename T> T read(std::istream &is) {
  T t;
  is >> t;
  return t;
}

using Field = std::vector<std::vector<bool>>; // true means occupied

void readToken(std::istream &is, std::string expectedToken) {
  std::string nextToken;
  is >> nextToken;
  if (nextToken != expectedToken) {
    std::cout << "Expected: " << expectedToken << ", actual: {" << nextToken
              << "}\n";
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
  std::ifstream ifs{filename};
  readToken(ifs, "version");
  readToken(ifs, "1");
  std::vector<Task> result;
  while (true) {
    if (ifs.eof() || ifs.fail() || ifs.bad())
      return result;
    int bucket = read<int>(ifs);
    std::string name = read<std::string>(ifs);
    int map_width = read<int>(ifs);
    int height = read<int>(ifs);
    int start_x = read<int>(ifs);
    int start_y = read<int>(ifs);
    int goal_x = read<int>(ifs);
    int goal_y = read<int>(ifs);
    double correctAnswer = read<double>(ifs);

    result.push_back(Task{bucket, name, map_width, height,
                          grid::GridPoint{start_x, start_y},
                          grid::GridPoint{goal_x, goal_y}, correctAnswer});
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

void displayPath(const Field &grid, const result::Path &path) {
  auto height = grid.size();
  auto width = grid[0].size();
  for (int y = 0; y < (int)height; ++y) {
    for (int x = 0; x < (int)width; ++x) {
      if (std::find(path.begin(), path.end(), grid::GridPoint{y, x}) !=
          path.end()) {
        std::cout << '$';
      } else {
        std::cout << (grid[y][x] ? '@' : '.');
      }
    }
    std::cout << std::endl;
  }
}

void displayTasks(std::string filename) {
  auto tasks = readTasksFromFile(filename);
  for (auto task : tasks) {
    std::cout << "task bucket=" << task.bucket << " from (" << task.start.x
              << ", " << task.start.y << ")" << " to (" << task.goal.x << ", "
              << task.goal.y << ")" << std::endl;
  }
  std::cout << "In total: " << tasks.size() << " entries\n";
}

grid::Grid::Cost manhattan(grid::GridPoint point1, grid::GridPoint point2) {
  return std::abs(point1.x - point2.x) + abs(point2.y - point1.y);
}

int main(int argc, char **argv) {
  argparse::ArgumentParser program("dstarlite-benchmarks");

  program.add_argument("--begin-x")
      .required()
      .help("one of the coordinates of the begin/end of the path")
      .scan<'i', int>();
  program.add_argument("--begin-y")
      .required()
      .help("one of the coordinates of the begin/end of the path")
      .scan<'i', int>();
  program.add_argument("--end-x")
      .required()
      .help("one of the coordinates of the begin/end of the path")
      .scan<'i', int>();
  program.add_argument("--end-y")
      .required()
      .help("one of the coordinates of the begin/end of the path")
      .scan<'i', int>();
  program.add_argument("--radius")
      .default_value(5)
      .help("visibility radius for a bot")
      .scan<'i', int>();
  program.add_argument("--generate-animations")
      .help("dump additional info needed to create beautiful gifs")
      .flag();
  program.add_argument("--map").help(
      "path to the file of format .map from moving ai");
  program.add_argument("--algorithm")
      .default_value("dstarlite")
      .help("one of: astar, dstarlite, dynswsffp");
  program.add_argument("--search-result-output")
      .default_value("./output/search-result.json");
  program.add_argument("--display-path").default_value(false).flag();
  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  const auto begin_y = program.get<int>("--begin-x");
  const auto begin_x = program.get<int>("--begin-y");
  const auto end_y = program.get<int>("--end-x");
  const auto end_x = program.get<int>("--end-y");
  const auto radius = program.get<int>("--radius");
  const auto mapPath = program.get<std::string>("--map");
  const auto algorithm = program.get<std::string>("--algorithm");
  const auto needDisplayPath = program.get<bool>("--display-path");

  const auto map = readMapFile(mapPath);
  grid::Grid grid(map);
  auto t1 = std::chrono::high_resolution_clock::now();
  const auto pathSearchResult = [&]() {
    auto gridView = grid::GridView(grid, radius);
    auto begin = grid::GridPoint{begin_x, begin_y};
    auto end = grid::GridPoint{end_x, end_y};
    if (algorithm == "dstarlite") {
      return unknownterrain::DStarLite::findShortestPath(
          gridView, begin, end, manhattan, "output/mytest");
    } else if (algorithm == "astar") {
      return unknownterrain::AStarReplanning::findShortestPath(gridView, begin,
                                                               end, manhattan);
    } else if (algorithm == "dynswsffp") {
      return unknownterrain::SWSFP::findShortestPath(gridView, begin, end);
    }
    throw std::runtime_error("unsupported algorithm: " + algorithm);
  }();
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> durationMs = t2 - t1;

  assert(pathSearchResult.pathFound);
  auto jsonPathSearchResult = json::object();
  jsonPathSearchResult["algorithm"] = algorithm;
  jsonPathSearchResult["time_millis"] = durationMs.count();
  jsonPathSearchResult["vertex_accesses"] = pathSearchResult.vertexAccesses;
  jsonPathSearchResult["array_accesses"] =
      pathSearchResult.priorityQueueAccesses;
  jsonPathSearchResult["path_length"] = pathSearchResult.path.size();
  const auto searchResultPath =
      program.get<std::string>("--search-result-output");
  std::ofstream ofjson(searchResultPath);
  ofjson << jsonPathSearchResult.dump(4);
  if (needDisplayPath) {
    displayPath(map, pathSearchResult.path);
  }
  return 0;
}