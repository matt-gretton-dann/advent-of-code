//
// Created by Matthew Gretton-Dann on 17/12/2022.
//

#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <regex>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

using namespace std::string_literals;
using Shape = std::vector<std::string>;

auto shapes_intersect(Shape const& tower, Shape const& shape, UInt x, UInt y) -> bool
{
  for (auto const& row : shape) {
    UInt cx{x};
    for (auto const& c : row) {
      if (c != ' ' && tower[y][cx] != '.') {
        return true;
      }
      ++cx;
    }
    ++y;
  }
  return false;
}

auto main() -> int
{
  std::string instructions;
  if (!std::getline(std::cin, instructions)) {
    std::cerr << "No input\n";
    return EXIT_FAILURE;
  }

  UInt height{0};
  std::string const floor{"+-------+"};
  std::string const wall{"|.......|"};
  Shape const horiz = {{"oooo"}};
  Shape const plus = {{" o "}, {"ooo"}, {" o "}};
  Shape const corner = {{"ooo"}, {"  o"}, {"  o"}};
  Shape const vert = {{"o"}, {"o"}, {"o"}, {"o"}};
  Shape const square = {{"oo"}, {"oo"}};
  std::vector<Shape> const shapes{horiz, plus, corner, vert, square};

  Shape tower{floor};
  auto wind_it{instructions.begin()};
  auto shape_it{shapes.begin()};
  for (UInt i{0}; i < 2022; ++i) {
    std::cout << " Shape " << i + 1 << " height: " << height << "\n";

    // Where should the block appear?  (2, 3 + height).
    UInt x{3};
    UInt y{4 + height};

    // Now make sure we have enough wall in the tower.
    while (tower.size() < y + shape_it->size() + 1) {
      tower.push_back(wall);
    }

    // Now do the instructions in turn:
    while (true) {
      // Wind movement
      UInt const trial_x{x + (*wind_it == '>' ? 1 : -1)};
      if (!shapes_intersect(tower, *shape_it, trial_x, y)) {
        x = trial_x;
      }

      ++wind_it;
      if (wind_it == instructions.end()) {
        wind_it = instructions.begin();
      }

      // Downward movement
      UInt const trial_y{y - 1};
      if (!shapes_intersect(tower, *shape_it, x, trial_y)) {
        y = trial_y;
      }
      else {
        break;
      }
    }

    // Insert the piece.
    for (auto const& r : *shape_it) {
      UInt cx{x};
      for (auto c : r) {
        if (c != ' ') {
          tower[y][cx] = '#';
        }
        ++cx;
      }
      ++y;
    }

    // Now find the height.
    height = tower.size() - 1;
    while (tower[height].find("#") == std::string::npos) {
      --height;
    }

    // Next shape
    ++shape_it;
    if (shape_it == shapes.end()) {
      shape_it = shapes.begin();
    }
  }
  std::cout << "Maximum height: " << height << "\n";
  return EXIT_SUCCESS;
}
