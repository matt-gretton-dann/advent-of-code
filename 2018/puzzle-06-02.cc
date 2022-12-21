//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <cstdlib>
#include <execution>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <utility>

using Int = std::int64_t;
using Point = std::pair<Int, Int>;

struct PointCompare
{
  auto operator()(Point const& lhs, Point const& rhs) const noexcept -> bool
  {
    if (lhs.first < rhs.first) {
      return true;
    }
    if (lhs.first > rhs.first) {
      return false;
    }
    return lhs.second < rhs.second;
  }
};

using PointMap = std::map<Point, Int, PointCompare>;

using namespace std::string_literals;

auto manhattan_distance(Point const& l, Point const& r) -> Int
{
  return std::abs(l.first - r.first) + std::abs(l.second - r.second);
}

template<typename iterator>
auto within_distance(iterator begin, iterator end, Point const& pt, Int distance) -> bool
{
  auto sum{std::accumulate(begin, end, Int{0}, [pt](Int a, auto const& p) {
    return a + manhattan_distance(p.first, pt);
  })};
  return sum < distance;
}

auto main() -> int
{
  std::string line;
  PointMap point_map;
  Point max(0, 0);
  std::regex const re{"(\\d+), (\\d+)"};

  Int const safe_distance{10'000};
  // At the edge of the grid (0,0) or (max,max) what is the largest possible sum of distances we
  // could have.
  Int edge_distance{0};

  // Read data
  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_match(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << "\n";
      return EXIT_FAILURE;
    }
    Point const pt(std::stoll(m.str(1)), std::stoll(m.str(2)));
    point_map.insert({pt, 0});
    edge_distance += pt.first + pt.second;
    max.first = std::max(max.first, pt.first);
    max.second = std::max(max.second, pt.second);
  }

  Int region_size{0};
  // Edge distance now becomes how far we have to go beyond the edge of the grid to ensure we count
  // all bits of the grid.
  edge_distance = (edge_distance > safe_distance) ? 0 : safe_distance - edge_distance;
  for (Int x{-edge_distance}; x <= max.first + edge_distance; ++x) {
    for (Int y{-edge_distance}; y <= max.second + edge_distance; ++y) {
      if (within_distance(point_map.begin(), point_map.end(), Point(x, y), safe_distance)) {
        ++region_size;
      }
    }
  }

  std::cout << "Safe size: " << region_size << "\n";
  return EXIT_SUCCESS;
}
