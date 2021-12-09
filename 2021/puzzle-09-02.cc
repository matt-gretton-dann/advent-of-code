//
// Created by mgretton on 09/12/2021.
//

#include <iostream>
#include <set>
#include <string>
#include <vector>

using Depths = std::vector<std::string>;
using Coord = std::pair<std::size_t, std::size_t>;
using Coords = std::set<Coord>;

auto find_basin_size(std::size_t x1, std::size_t y1, Depths const& depths) -> std::size_t
{
  Coords visited;
  Coords to_visit;
  to_visit.insert({x1, y1});
  while (!to_visit.empty()) {
    Coords next_to_visit;
    for (auto const& coord : to_visit) {
      auto x{coord.first};
      auto y{coord.second};
      if (x > 0 && depths[y][x - 1] != '9' && !visited.contains({x - 1, y})) {
        next_to_visit.insert({x - 1, y});
        visited.insert({x - 1, y});
      }
      if (y > 0 && depths[y - 1][x] != '9' && !visited.contains({x, y - 1})) {
        next_to_visit.insert({x, y - 1});
        visited.insert({x, y - 1});
      }
      if (x < depths[y].size() - 1 && depths[y][x + 1] != '9' && !visited.contains({x + 1, y})) {
        next_to_visit.insert({x + 1, y});
        visited.insert({x + 1, y});
      }
      if (y < depths.size() - 1 && depths[y + 1][x] != '9' && !visited.contains({x, y + 1})) {
        next_to_visit.insert({x, y + 1});
        visited.insert({x, y + 1});
      }
    }
    std::swap(next_to_visit, to_visit);
  }

  return visited.size();
}

auto main() -> int
{
  Depths depths;
  std::string line;
  while (std::getline(std::cin, line)) {
    depths.push_back(line);
  }

  std::vector<std::size_t> basin_sizes;
  for (std::size_t y{0}; y < depths.size(); ++y) {
    for (std::size_t x{0}; x < depths[y].size(); ++x) {
      bool min = true;
      min &= (x == 0) || (depths[y][x - 1] > depths[y][x]);
      min &= (y == 0) || (depths[y - 1][x] > depths[y][x]);
      min &= (x == depths[y].size() - 1) || (depths[y][x + 1] > depths[y][x]);
      min &= (y == depths.size() - 1) || (depths[y + 1][x] > depths[y][x]);
      if (min) {
        basin_sizes.push_back(find_basin_size(x, y, depths));
      }
    }
  }

  std::sort(basin_sizes.begin(), basin_sizes.end());
  std::reverse(basin_sizes.begin(), basin_sizes.end());
  std::cout << "Result " << basin_sizes[0] << " * " << basin_sizes[1] << " * " << basin_sizes[2]
            << " = " << basin_sizes[0] * basin_sizes[1] * basin_sizes[2] << '\n';
  return 0;
}
