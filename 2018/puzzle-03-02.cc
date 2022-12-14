//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cstdlib>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "position.h"

using Int = std::int64_t;

struct Grid
{
  [[nodiscard]] auto width() const noexcept -> Int { return width_; }
  [[nodiscard]] auto height() const noexcept -> Int { return static_cast<Int>(grid_.size()); }

  auto add_rect(Pos<Int> tl, Pos<Int> br, Int value)
  {
    unique_.insert(value);
    for (Int x{std::min(tl.x(), br.x())}; x < std::max(tl.x(), br.x()); ++x) {
      for (Int y{std::min(tl.y(), br.y())}; y < std::max(tl.y(), br.y()); ++y) {
        set_point(Pos<Int>{x, y}, value);
      }
    }
  }

  auto unique() const noexcept -> Int
  {
    assert(unique_.size() == 1);
    return *unique_.begin();
  }

private:
  auto set_point(Pos<Int> pos, Int value) -> void
  {
    auto const x{pos.x()};
    auto const y{pos.y()};

    if (y >= height()) {
      grid_.resize(y + 1);
    }
    if (x >= grid_[y].size()) {
      grid_[y].resize(x + 1, 0);
      width_ = std::max(width_, x);
    }
    if (grid_[y][x] != 0) {
      unique_.erase(grid_[y][x]);
      unique_.erase(value);
    }
    grid_[y][x] = value;
  }

  std::vector<std::vector<Int>> grid_;
  Int width_{0};
  std::set<Int> unique_;
};

auto main() -> int
{
  std::string line;
  Grid grid;

  while (std::getline(std::cin, line)) {
    assert(line[0] == '#');
    std::size_t pos{1};
    std::size_t l{0};
    Int const id{std::stoll(line.substr(pos), &l)};
    pos += l + 3;
    Int const tlx{std::stoll(line.substr(pos), &l)};
    pos += l + 1;
    Int const tly{std::stoll(line.substr(pos), &l)};
    pos += l + 2;
    Int const w{std::stoll(line.substr(pos), &l)};
    pos += l + 1;
    Int const h{std::stoll(line.substr(pos), &l)};
    grid.add_rect(Pos<Int>{tlx, tly}, Pos<Int>{tlx + w, tly + h}, id);
  }

  std::cout << "Unique: " << grid.unique() << '\n';

  return EXIT_SUCCESS;
}
