//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cstdlib>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "position.h"
#include <cassert>

using Int = std::int64_t;

struct Grid
{
  [[nodiscard]] auto width() const noexcept -> Int { return width_; }
  [[nodiscard]] auto height() const noexcept -> Int { return static_cast<Int>(grid_.size()); }

  auto add_rect(Pos<Int> tl, Pos<Int> br)
  {
    for (Int x{std::min(tl.x(), br.x())}; x < std::max(tl.x(), br.x()); ++x) {
      for (Int y{std::min(tl.y(), br.y())}; y < std::max(tl.y(), br.y()); ++y) {
        incr_point(Pos<Int>{x, y});
      }
    }
  }

  auto double_counted() const noexcept -> Int
  {
    Int result{0};
    for (auto const& r : grid_) {
      for (auto c : r) {
        if (c > 1) {
          ++result;
        }
      }
    }
    return result;
  }

private:
  auto incr_point(Pos<Int> pos) -> void
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
    ++grid_[y][x];
  }

  std::vector<std::vector<Int>> grid_;
  Int width_{0};
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
    grid.add_rect(Pos<Int>{tlx, tly}, Pos<Int>{tlx + w, tly + h});
  }

  std::cout << "Double counted: " << grid.double_counted() << '\n';

  return EXIT_SUCCESS;
}
