//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <cstdlib>
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
auto find_closest(iterator begin, iterator end, Point const& pt) -> iterator
{
  iterator closest{end};
  Int min_distance{std::numeric_limits<Int>::max()};
  for (; begin != end; ++begin) {
    auto dist{manhattan_distance(begin->first, pt)};
    if (dist < min_distance) {
      min_distance = dist;
      closest = begin;
    }
    else if (dist == min_distance) {
      closest = end;
    }
  }

  return closest;
}

auto main() -> int
{
  std::string line;
  PointMap point_map;
  Point max(0, 0);
  std::regex const re{"(\\d+), (\\d+)"};

  // Read data
  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_match(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << "\n";
      return EXIT_FAILURE;
    }
    Point const pt(std::stoull(m.str(1)), std::stoull(m.str(2)));
    point_map.insert({pt, 0});
    max.first = std::max(max.first, pt.first);
    max.second = std::max(max.second, pt.second);
  }

  // Examine each point within the grid.
  for (Int x{1}; x <= max.first; ++x) {
    for (Int y{1}; y <= max.second; ++y) {
      auto closest_point = find_closest(point_map.begin(), point_map.end(), Point(x, y));
      if (closest_point != point_map.end()) {
        closest_point->second += 1;
      }
    }
  }

  // Now do the top & bottom edges to detect infinities.
  for (Int x{0}; x <= max.first; ++x) {
    auto closest_point = find_closest(point_map.begin(), point_map.end(), Point(x, 0));
    if (closest_point != point_map.end()) {
      closest_point->second = 0;
    }
    closest_point = find_closest(point_map.begin(), point_map.end(), Point(x, max.second + 1));
    if (closest_point != point_map.end()) {
      closest_point->second = 0;
    }
  }

  // Now do the left & right edges to detect infinities.
  for (Int y{0}; y <= max.second; ++y) {
    auto closest_point = find_closest(point_map.begin(), point_map.end(), Point(0, y));
    if (closest_point != point_map.end()) {
      closest_point->second = 0;
    }
    closest_point = find_closest(point_map.begin(), point_map.end(), Point(max.first, y));
    if (closest_point != point_map.end()) {
      closest_point->second = 0;
    }
  }

  // Find maximum size.
  auto closest_size{
    std::max_element(point_map.begin(), point_map.end(),
                     [](auto const& l, auto const& r) { return l.second < r.second; })};
  std::cout << "Point: " << closest_size->first.first << ", " << closest_size->first.second << "\n";
  std::cout << "Safe size: " << closest_size->second << "\n";
  return EXIT_SUCCESS;
}
