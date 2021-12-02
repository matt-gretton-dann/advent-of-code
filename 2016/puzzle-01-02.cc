//
// Created by Matthew Gretton-Dann on 02/12/2021.
//

#include <iostream>
#include <set>
#include <string>

enum Direction { north, east, south, west };
using Location = std::pair<int, int>;

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Failed to read line.\n";
    return 1;
  }

  std::set<Location> visited;
  Location current{0, 0};
  Location direction{0, 1};
  visited.insert(current);

  for (std::size_t pos{0}; pos < line.size(); pos += 2) {
    switch (line.at(pos)) {
    case 'L':
      direction = {-direction.second, direction.first};
      break;
    case 'R':
      direction = {direction.second, -direction.first};
      break;
    default:
      abort();
    }

    ++pos;
    assert(pos < line.size());
    std::size_t idx{0};
    auto dist{static_cast<int>(std::stoul(line.substr(pos), &idx))};
    while (dist-- > 0) {
      current.first += direction.first;
      current.second += direction.second;
      if (visited.find(current) != visited.end()) {
        auto horiz = std::abs(current.first);
        auto vert = std::abs(current.second);
        std::cout << "We end up " << horiz << " + " << vert << " = " << horiz + vert
                  << " blocks away.\n";
        return 0;
      }
      visited.insert(current);
    }

    pos += idx;
  }
  std::cerr << "Didn't find an answer.\n";
  return 1;
}