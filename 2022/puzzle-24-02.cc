//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <cassert>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <set>
#include <stdexcept>
#include <utility>

using namespace std::string_literals;

using Int = std::int64_t;
using UInt = std::uint64_t;

using Point = std::pair<Int, Int>;
using Blizard = std::pair<Point, Point>;

constexpr Point up{0, -1};
constexpr Point right{1, 0};
constexpr Point down{0, 1};
constexpr Point left{-1, 0};

struct BlizzardCompare
{
  [[nodiscard]] constexpr auto operator()(Point const& lhs, Point const& rhs) const noexcept -> bool
  {
    return (lhs.first < rhs.first) || (lhs.first == rhs.first && lhs.second < rhs.second);
  }

  [[nodiscard]] constexpr auto operator()(Blizard const& lhs, Blizard const& rhs) const noexcept
    -> bool
  {
    return (*this)(lhs.first, rhs.first);
  }

  [[nodiscard]] constexpr auto operator()(Blizard const& lhs, Point const& rhs) const noexcept
    -> bool
  {
    return this->operator()(lhs.first, rhs);
  }

  [[nodiscard]] constexpr auto operator()(Point const& lhs, Blizard const& rhs) const noexcept
    -> bool
  {
    return this->operator()(lhs, rhs.first);
  }
};

using Blizards = std::multiset<Blizard, BlizzardCompare>;

struct State
{
  Point pt_{0, 0};
  std::size_t dest_{0};

  [[nodiscard]] constexpr auto operator<(State const& rhs) const noexcept -> bool
  {
    return pt_ < rhs.pt_ || (pt_ == rhs.pt_ && dest_ < rhs.dest_);
  }
};

auto step_time(Blizards const& blizzards, Point size) -> Blizards
{
  Blizards result;
  for (auto const& blizzard : blizzards) {
    Blizard new_blizzard{{blizzard.first.first + blizzard.second.first,
                          blizzard.first.second + blizzard.second.second},
                         blizzard.second};
    if (new_blizzard.first.first == 0) {
      new_blizzard.first.first = size.first - 1;
    }
    if (new_blizzard.first.first == size.first) {
      new_blizzard.first.first = 1;
    }
    if (new_blizzard.first.second == 0) {
      new_blizzard.first.second = size.second - 1;
    }
    if (new_blizzard.first.second == size.second) {
      new_blizzard.first.second = 1;
    }
    result.insert(new_blizzard);
  }
  return result;
}

auto main() -> int
{
  std::string line;
  Blizards blizzards;

  // We surround each line of the grid with extra dots, so we don't have to resize as we go along.
  Point size{0, -1};
  while (std::getline(std::cin, line)) {
    if (size.first == 0) {
      size.first = static_cast<Int>(line.size()) - 1;
    }
    assert(size.first == line.size() - 1);
    ++size.second;
    Point pos{0, size.second};
    for (auto const c : line) {
      switch (c) {
      case '>':
        blizzards.insert({pos, right});
        break;
      case 'v':
        blizzards.insert({pos, down});
        break;
      case '<':
        blizzards.insert({pos, left});
        break;
      case '^':
        blizzards.insert({pos, up});
        break;
      case '#':
      case '.':
        break;
      default:
        abort();
      }
      pos.first += 1;
    }
  }

  Point const initial{1, 0};
  Point const target{size.first - 1, size.second};
  std::array<Point, 3> targets{target, initial, target};
  State const initial_state{initial, 0};
  std::set<State> next_points{initial_state};
  std::array<Point, 5> const dirs{up, right, down, left, {0, 0}};
  UInt turn_count{0};
  while (true) {
    blizzards = step_time(blizzards, size);

    std::set<State> const points{next_points};
    next_points.clear();

    assert(!points.empty());
    for (auto const& pt : points) {
      auto dest{pt.dest_};
      if (pt.pt_ == targets.at(dest)) {
        dest += 1;
        if (dest == targets.size()) {
          std::cout << "Number of turns: " << turn_count << "\n";
          return EXIT_SUCCESS;
        }
      }

      for (auto const& dir : dirs) {
        Point const proposed{pt.pt_.first + dir.first, pt.pt_.second + dir.second};
        auto it{blizzards.find({proposed, dir})};
        if (it != blizzards.end()) {
          continue;
        }
        if (proposed.first <= 0) {
          continue;
        }
        if (proposed.first >= size.first) {
          continue;
        }
        if (proposed.second <= 0 && proposed != Point{1, 0}) {
          continue;
        }
        if (proposed.second >= size.second && proposed != target) {
          continue;
        }
        next_points.insert({proposed, dest});
      }
    }

    ++turn_count;
  }
}
