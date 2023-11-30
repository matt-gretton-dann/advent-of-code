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
using Data = std::pair<UInt, bool>;

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
  std::map<Pos, Data, compare_pos> trees;

  while (std::getline(std::cin, line) && !line.empty()) {
    Pos pos{0, grid_size.imag()};
    assert(grid_size.real() == 0 || grid_size.real() == line.size());
    for (auto c : line) {
      trees.insert({pos, Data{static_cast<UInt>(c - '0'), false}});
      pos += 1;
    }
    grid_size = pos + Pos{0, 1};
  }

  for (UInt j = 0; j < grid_size.imag(); ++j) {
    UInt min{0};
    for (UInt i{0}; i < grid_size.real(); ++i) {
      auto it{trees.find(Pos{i, j})};
      assert(it != trees.end());
      if (it->second.first >= min) {
        min = it->second.first + 1;
        it->second.second = true;
      }
    }
    min = 0;
    for (UInt i{grid_size.real()}; i > 0; --i) {
      auto it{trees.find(Pos{i - 1, j})};
      assert(it != trees.end());
      if (it->second.first >= min) {
        min = it->second.first + 1;
        it->second.second = true;
      }
    }
  }

  for (UInt i = 0; i < grid_size.real(); ++i) {
    UInt min{0};
    for (UInt j{0}; j < grid_size.imag(); ++j) {
      auto it{trees.find(Pos{i, j})};
      assert(it != trees.end());
      if (it->second.first >= min) {
        min = it->second.first + 1;
        it->second.second = true;
      }
    }
    min = 0;
    for (UInt j{grid_size.imag()}; j > 0; --j) {
      auto it{trees.find(Pos{i, j - 1})};
      assert(it != trees.end());
      if (it->second.first >= min) {
        min = it->second.first + 1;
        it->second.second = true;
      }
    }
  }

  auto visible_count{
    std::accumulate(trees.begin(), trees.end(), UInt{0},
                    [](UInt a, auto const& node) { return node.second.second ? (a + 1) : a; })};
  std::cout << "Number visible " << visible_count << '\n';
#if 0
  for (UInt j{0}; j != grid_size.imag(); ++j) {
    for (UInt i{0}; i != grid_size.imag(); ++i) {
      std::cout << trees.find({i, j})->second.first;
    }
    std::cout << '\n';
  }
  for (UInt j{0}; j != grid_size.imag(); ++j) {
    for (UInt i{0}; i != grid_size.imag(); ++i) {
      std::cout << (trees.find({i, j})->second.second ? '*' : '.');
    }
    std::cout << '\n';
  }
#endif
  return 0;
}
