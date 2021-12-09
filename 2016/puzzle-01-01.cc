//
// Created by Matthew Gretton-Dann on 02/12/2021.
//

#include <cassert>
#include <iostream>
#include <string>

enum Direction { north, east, south, west };

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Failed to read line.\n";
    return 1;
  }

  Direction direction{north};
  int horiz{0};
  int vert{0};
  for (std::size_t pos{0}; pos < line.size(); pos += 2) {
    switch (line.at(pos)) {
    case 'L':
      if (direction == Direction::north) {
        direction = Direction::west;
      }
      else {
        direction = static_cast<Direction>(static_cast<int>(direction) - 1);
      }
      break;
    case 'R':
      if (direction == Direction::west) {
        direction = Direction::north;
      }
      else {
        direction = static_cast<Direction>(static_cast<int>(direction) + 1);
      }
      break;
    default:
      abort();
    }
    ++pos;
    assert(pos < line.size());
    std::size_t idx{0};
    auto dist{static_cast<int>(std::stoul(line.substr(pos), &idx))};
    pos += idx;
    switch (direction) {
    case Direction::north:
      vert += dist;
      break;
    case Direction::east:
      horiz += dist;
      break;
    case Direction::south:
      vert -= dist;
      break;
    case Direction::west:
      horiz -= dist;
      break;
    default:
      abort();
    }
  }

  horiz = std::abs(horiz);
  vert = std::abs(vert);
  std::cout << "We end up " << horiz << " + " << vert << " = " << horiz + vert << " blocks away.\n";
  return 0;
}
