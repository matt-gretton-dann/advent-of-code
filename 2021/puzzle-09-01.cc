//
// Created by mgretton on 09/12/2021.
//

#include <iostream>
#include <string>
#include <vector>

using Depths = std::vector<std::string>;

auto main() -> int
{
  Depths depths;
  std::string line;
  while (std::getline(std::cin, line)) {
    depths.push_back(line);
  }

  unsigned risk{0};
  for (std::size_t y{0}; y < depths.size(); ++y) {
    for (std::size_t x{0}; x < depths[y].size(); ++x) {
      bool min = true;
      min &= (x == 0) || (depths[y][x - 1] > depths[y][x]);
      min &= (y == 0) || (depths[y - 1][x] > depths[y][x]);
      min &= (x == depths[y].size() - 1) || (depths[y][x + 1] > depths[y][x]);
      min &= (y == depths.size() - 1) || (depths[y + 1][x] > depths[y][x]);
      if (min) {
        risk += depths[y][x] - '0' + 1;
      }
    }
  }

  std::cout << "Risk factor " << risk << '\n';
  return 0;
}
