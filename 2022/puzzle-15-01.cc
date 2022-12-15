//
// Created by Matthew Gretton-Dann on 09/12/2022.
//

#include <array>
#include <iostream>
#include <list>
#include <numeric>
#include <regex>
#include <set>
#include <utility>

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
  Int const row{2000000};
  std::regex const re{
    R"(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))"};
  std::string line;
  std::set<Range, RangeCompare> ranges;
  std::set<Int> beacons_on_row;

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

    // Distance between sensor and beacon:
    Int const dist{std::abs(sx - bx) + std::abs(sy - by)};
    // Distance(ish) to row we're interested in:
    Int const row_dist{std::abs(sy - row)};
    std::cout << "Sensor (" << sx << ", " << sy << "):\n"
              << "  Nearest beacon is: " << bx << ", " << by << "\n"
              << "  Distance: " << dist << "\n"
              << "  y-Distance to row " << row << ": " << row_dist << "\n";
    // If that distance is non-negative then we have places the beacon can't be
    if (row_dist <= dist) {
      Int const x_dist{dist - row_dist};
      std::cout << "  x distance left: " << x_dist << "\n"
                << "  Range: " << sx - x_dist << " - " << sx + x_dist + 1 << "\n";
      Range const missing{sx - x_dist, sx + x_dist + 1};
      ranges.insert(missing);
    }
    if (by == row) {
      std::cout << "  Beacon on row.\n";
      beacons_on_row.insert(bx);
    }
  }

  Int last_pos{std::numeric_limits<Int>::min()};
  UInt count{0};
  std::cout << "\n\n\n";
  for (auto const& r : ranges) {
    assert(r.first < r.second);
    std::cout << "Range: " << r.first << " - " << r.second << "\n";
    if (r.second < last_pos) {
      std::cout << "  Skipping\n";
      continue;
    }
    auto b{std::max(r.first, last_pos)};
    std::cout << "  Taking range start as: " << b << '\n'
              << "  Adding to count: " << r.second - b << "\n";
    count += (r.second - b);
    last_pos = r.second;
  }

  std::cout << "Removing beacons we know are on the row: " << beacons_on_row.size() << "\n";
  count -= beacons_on_row.size();

  std::cout << "No beacons in: " << count << " places.\n";
  return EXIT_SUCCESS;
}
