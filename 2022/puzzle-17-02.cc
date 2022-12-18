//
// Created by Matthew Gretton-Dann on 17/12/2022.
//

#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <regex>
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

auto tower_height(Shape const& tower) -> UInt
{
  UInt height{tower.size() - 1};
  while (tower[height].find('#') == std::string::npos) {
    --height;
  }
  return height;
}

auto main() -> int
{
  std::string instructions;
  if (!std::getline(std::cin, instructions)) {
    std::cerr << "No input\n";
    return EXIT_FAILURE;
  }

  std::string const floor{"#-------#"};
  std::string const wall{"|.......|"};
  Shape const horiz = {{"oooo"}};
  Shape const plus = {{" o "}, {"ooo"}, {" o "}};
  Shape const corner = {{"ooo"}, {"  o"}, {"  o"}};
  Shape const vert = {{"o"}, {"o"}, {"o"}, {"o"}};
  Shape const square = {{"oo"}, {"oo"}};
  std::vector<Shape> const shapes{horiz, plus, corner, vert, square};

  // Numbers we want to store for later
  std::vector<UInt> heights;  // heights[i] gives height of tower before piece i falls.
  std::map<UInt, UInt>
    wind_iter_map;  // Maps place in wind string vs iteration.  Used to see when we start repeating.

  // We have a ridiculous number of iterations to do - more than we can do on this machine so we
  // are going to assume that we have three phases: Setup, repeated, Teradown
  // We identify the repeated section by being at the same place in the instructions for Shape 0 as
  // we have been before.
  UInt previous_repeated_height{0};

  Shape tower{floor};
  auto wind_it{instructions.begin()};
  auto shape_it{shapes.begin()};
  UInt constexpr max_iter{1000000000000};
  for (UInt i{0}; i < max_iter; ++i) {
    UInt const height{tower_height(tower)};
    heights.push_back(height);

    if (shape_it == shapes.begin()) {
      // See if we've seen this wind position before for shape 0.
      auto [it, success] = wind_iter_map.insert({wind_it - instructions.begin(), i});
      if (!success) {
        // We have - what is the delta in height since the last time?  If it is the same delta as
        // the previous repeat we saw, then we can stop iterating.
        UInt const repeated_height(heights[it->second] - heights[i]);
        std::cout << "Repeated wind instruction " << it->first << ": Iters [" << it->second << ", "
                  << i << ").  Heights: [" << heights[it->second] << ", " << heights[i]
                  << "].  Difference: " << heights[i] - heights[it->second] << ".\n";
        if (repeated_height == previous_repeated_height) {
          break;
        }
        previous_repeated_height = repeated_height;
        it->second = i;
      }
    }

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

    // Next shape
    ++shape_it;
    if (shape_it == shapes.end()) {
      shape_it = shapes.begin();
    }
  }

  UInt const repeat_start{wind_iter_map.find(wind_it - instructions.begin())->second};
  UInt const repeat_end{heights.size() - 1};
  // [0, repeat_start) is the prologue. [repeat_start, repeat_end) is repeated heights added.
  // [repeat_end, max_iter) is the epilogue.
  UInt const repeat_length{repeat_end - repeat_start};
  UInt const num_repeats{(max_iter - repeat_start) / repeat_length};
  UInt const num_left = max_iter - repeat_length * num_repeats;
  UInt const height =
    heights[num_left] + (heights[repeat_end] - heights[repeat_start]) * num_repeats;
  std::cout << "Maximum height: " << height << "\n";
  return EXIT_SUCCESS;
}
