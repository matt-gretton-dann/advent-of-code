//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <regex>
#include <utility>

constexpr std::size_t grid_size{300};
using Int = std::int64_t;
using Grid = std::array<std::array<Int, grid_size>, grid_size>;
using SmallGrid = std::array<std::array<Int, grid_size - 2>, grid_size - 2>;

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return EXIT_FAILURE;
  }

  Int const serial{std::stoll(line)};
  Grid grid;
  for (Int y{0}; y < grid_size; ++y) {
    for (Int x{0}; x < grid_size; ++x) {
      Int const rack_id{x + 1 + 10};
      Int power{(rack_id * (y + 1) + serial) * rack_id};
      power /= 100;
      power %= 10;
      power -= 5;
      grid.at(y).at(x) = power;
    }
  }

  Int max_power{std::numeric_limits<Int>::min()};
  Int max_x{0};
  Int max_y{0};
  Int max_g{0};

  Grid small_grid{grid};

  for (Int g{0}; g < grid_size; ++g) {
    Int sgs(grid_size - g);
    for (Int y{0}; y < sgs; ++y) {
      for (Int x{0}; x < sgs; ++x) {
        for (Int dx{0}; dx <= g; ++dx) {
          small_grid[y][x] += grid[y + g][x + dx];
        }
        for (Int dy{0}; dy < g; ++dy) {
          small_grid[y][x] += grid[y + dy][x + g];
        }

        if (max_power < small_grid[y][x]) {
          max_power = small_grid[y][x];
          max_x = x + 1;
          max_y = y + 1;
          max_g = g + 1;
        }
      }
    }
  }
  std::cout << "Maximum power: " << max_power << "\n";
  std::cout << "Coordinates: " << max_x << "," << max_y << "," << max_g << "\n";

  return EXIT_SUCCESS;
}
