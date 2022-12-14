//
// Created by Matthew Gretton-Dann on 09/12/2022.
//

#include <array>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "position.h"

using Int = std::int64_t;
using UInt = std::uint64_t;

auto direction(auto x) -> decltype(x)
{
  if (x < 0) {
    return -1;
  }
  if (x > 0) {
    return 1;
  }
  return 0;
}

struct Map
{
  static constexpr char gap_ = '.';
  static constexpr char wall_ = '#';
  static constexpr char sand_ = 'o';
  static constexpr Pos<Int> sand_start_{500, 0};

  [[nodiscard]] auto height() const noexcept -> Int { return static_cast<Int>(map_.size()); }
  [[nodiscard]] auto width() const noexcept -> Int { return width_; }

  [[nodiscard]] auto at(Pos<Int> const& p) const noexcept -> char
  {
    if (p.y() >= height()) {
      return gap_;
    }
    if (p.x() >= map_[p.y()].size()) {
      return gap_;
    }
    return map_[p.y()][p.x()];
  }

  auto add_point(Pos<Int> const& p, char val = wall_)
  {
    auto const x{p.x()};
    auto const y{p.y()};
    if (height() <= p.y()) {
      map_.resize(p.y() + 1);
    }
    if (map_[y].size() < x + 1) {
      map_[y].resize(x + 1, gap_);
      width_ = std::max(width_, x + 1);
    }
    map_[y][x] = val;
    min_x_ = std::min(min_x_, x);
  }

  auto add_sand() -> bool
  {
    Pos pos{sand_start_};
    while (true) {
      Pos const old_pos{pos};
      if (pos.y() >= height()) {
        return false;
      }
      pos += Pos<Int>{0, 1};
      if (at(pos) == gap_) {
        continue;
      }
      pos += Pos<Int>{-1, 0};
      if (at(pos) == gap_) {
        continue;
      }
      pos += Pos<Int>{2, 0};
      if (at(pos) == gap_) {
        continue;
      }
      add_point(old_pos, sand_);
      return true;
    }
  }

  auto add_wall(Pos<Int> start, Pos<Int> const& end)
  {
    Pos<Int> const delta{direction(end.x() - start.x()), direction(end.y() - start.y())};
    for (; start != end; start += delta) {
      add_point(start);
    }
    add_point(end);
  }

  auto dump() const
  {
    std::cout << "First displayed column: " << min_x_ << '\n';
    for (auto row : map_) {
      row.resize(width_, gap_);
      std::cout << row.substr(min_x_) << '\n';
    }
  }

private:
  std::vector<std::string> map_;
  Int width_{0};
  Int min_x_{std::numeric_limits<Int>::max()};
};

auto main() -> int
{
  std::string line;
  Map map;

  while (std::getline(std::cin, line)) {
    std::size_t pos{0};
    std::size_t l{0};
    Pos<Int> start{0, 0};
    start.x(std::stoll(line.substr(pos), &l));
    pos += l + 1;
    start.y(std::stoll(line.substr(pos), &l));
    pos += l + 4;

    while (pos < line.size()) {
      Pos<Int> end{0, 0};
      end.x(std::stoll(line.substr(pos), &l));
      pos += l + 1;
      end.y(std::stoll(line.substr(pos), &l));
      pos += l + 4;
      map.add_wall(start, end);
      start = end;
    }
  }

  Int sand_pieces{0};
  map.dump();
  while (map.add_sand()) {
    map.dump();
    ++sand_pieces;
  }

  std::cout << "Number of pieces of sand: " << sand_pieces << '\n';

  return 0;
}
