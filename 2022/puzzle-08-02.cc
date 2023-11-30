//
// Created by Matthew Gretton-Dann on 08/12/2022.
//

#include <cassert>
#include <complex>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <utility>

using UInt = std::uint64_t;
using Pos = std::complex<UInt>;

struct compare_pos
{
  constexpr auto operator()(Pos const& l, Pos const& r) const noexcept
  {
    if (l.imag() < r.imag()) {
      return true;
    }
    if (l.imag() == r.imag() && l.real() < r.real()) {
      return true;
    }
    return false;
  }
};

auto main() -> int
{
  std::string line;
  Pos grid_size{0, 0};
  std::map<Pos, UInt, compare_pos> trees;

  while (std::getline(std::cin, line) && !line.empty()) {
    Pos pos{0, grid_size.imag()};
    assert(grid_size.real() == 0 || grid_size.real() == line.size());
    for (auto c : line) {
      trees.insert({pos, static_cast<UInt>(c - '0')});
      pos += 1;
    }
    grid_size = pos + Pos{0, 1};
  }

  UInt max_scenic_score{0};
  // All edges score 0.
  for (UInt x{1}; x < grid_size.real() - 1; ++x) {
    for (UInt y{1}; y < grid_size.imag() - 1; ++y) {
      std::cout << "(" << x << ", " << y << "):";
      UInt const tree_height{trees.find({x, y})->second};
      UInt scenic_score{1};
      UInt i{x + 1};
      for (; i < grid_size.real(); ++i) {
        if (trees.find({i, y})->second >= tree_height) {
          break;
        }
      }
      scenic_score *= i - x - static_cast<UInt>(i == grid_size.real());
      std::cout << i - x - static_cast<UInt>(i == grid_size.real()) << " * ";

      for (i = x; i > 0; --i) {
        if (trees.find({i - 1, y})->second >= tree_height) {
          break;
        }
      }
      scenic_score *= x - i + static_cast<UInt>(i != 0);
      std::cout << x - i + static_cast<UInt>(i != 0) << " * ";

      UInt j{y + 1};
      for (; j < grid_size.imag(); ++j) {
        if (trees.find({x, j})->second >= tree_height) {
          break;
        }
      }
      scenic_score *= j - y - static_cast<UInt>(j == grid_size.imag());
      std::cout << j - y - static_cast<UInt>(j == grid_size.imag()) << " * ";

      for (j = y; j > 0; --j) {
        if (trees.find({x, j - 1})->second >= tree_height) {
          break;
        }
      }
      scenic_score *= y - j + static_cast<UInt>(j != 0);
      std::cout << y - j + static_cast<UInt>(j != 0);

      std::cout << " = " << scenic_score << '\n';
      max_scenic_score = std::max(max_scenic_score, scenic_score);
    }
  }
  std::cout << "Mac scenic score " << max_scenic_score << '\n';
  return 0;
}
