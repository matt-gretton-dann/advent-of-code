//
// Created by Matthew Gretton-Dann on 09/12/2022.
//

#include <array>
#include <iostream>
#include <list>
#include <numeric>
#include <regex>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;
using Range = std::pair<Int, Int>;

struct RangeCompare
{
  auto operator()(Range const& l, Range const& r) const noexcept -> bool
  {
    if (l.first < r.first) {
      return true;
    }
    if (l.first == r.first && l.second < r.second) {
      return true;
    }
    return false;
  }
};

auto main() -> int
{
  Int const max_row{4000000 + 1};
  std::regex const re{
    R"(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))"};
  std::string line;
  std::vector<std::pair<Range, Range>> sensors;
  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << "\n";
      return EXIT_FAILURE;
    }
    Int const sx{std::stoll(m.str(1))};
    Int const sy{std::stoll(m.str(2))};
    Int const bx{std::stoll(m.str(3))};
    Int const by{std::stoll(m.str(4))};
    sensors.push_back({{sx, sy}, {bx, by}});
  }

  for (Int row{0}; row < max_row; ++row) {
    std::vector<Range> ranges;
    ranges.reserve(sensors.size());

    for (auto const& sensor : sensors) {
      Int const sx{sensor.first.first};
      Int const sy{sensor.first.second};
      Int const bx{sensor.second.first};
      Int const by{sensor.second.second};

      // Distance between sensor and beacon:
      Int const dist{std::abs(sx - bx) + std::abs(sy - by)};
      // Distance(ish) to row we're interested in:
      Int const row_dist{std::abs(sy - row)};
      // If that distance is non-negative then we have places the beacon can't be
      if (row_dist <= dist) {
        Int const x_dist{dist - row_dist};
        Range const missing{sx - x_dist, sx + x_dist + 1};
        ranges.emplace_back(missing);
      }
    }
    std::sort(ranges.begin(), ranges.end(), RangeCompare{});

    Int last_pos{0};
    for (auto const& r : ranges) {
      assert(r.first < r.second);
      if (r.second < last_pos) {
        continue;
      }
      if (r.first > last_pos) {
        std::cout << " Gap in " << last_pos << " - " << r.first << "\n";
        std::cout << "Score = " << last_pos * 4000000 + row << "\n";
        return EXIT_SUCCESS;
      }

      last_pos = std::min(r.second, max_row);
    }
  }
  return EXIT_FAILURE;
}
