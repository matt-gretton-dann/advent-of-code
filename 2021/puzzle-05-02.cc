//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>

using Point = std::pair<long, long>;  // NOLINT(google-runtime-int)

auto get_points(std::string const& s) -> std::pair<Point, Point>
{
  static std::regex re{R"((\d+),(\d+) -> (\d+),(\d+))"};
  std::smatch m;
  if (!std::regex_search(s, m, re)) {
    std::cerr << "Unable to interpret: " << s << "\n";
    std::exit(1);  // NOLINT(concurrency-mt-unsafe)
  }
  return {{std::stol(m.str(1)), std::stol(m.str(2))}, {std::stol(m.str(3)), std::stol(m.str(4))}};
}

auto sign(long x) -> long  // NOLINT(google-runtime-int)
{
  if (x == 0) {
    return 0;
  }
  if (x > 0) {
    return 1;
  }
  return -1;
}

auto get_delta(Point const& pt1, Point const& pt2) -> Point
{
  return {sign(pt2.first - pt1.first), sign(pt2.second - pt1.second)};
}

auto main() -> int
{
  std::string line;
  std::map<Point, unsigned> vents;
  while (std::getline(std::cin, line)) {
    auto [pt1, pt2] = get_points(line);
    auto delta{get_delta(pt1, pt2)};
    assert(std::abs(pt1.first - pt2.first) == std::abs(pt1.second - pt2.second) ||
           pt1.first == pt2.first || pt1.second == pt2.second);
    std::cout << "{" << pt1.first << ", " << pt1.second << "} -> {" << pt2.first << ", "
              << pt2.second << "} - delta {" << delta.first << ", " << delta.second << "}\n";
    while (pt1 != pt2) {
      auto [it, success] = vents.insert({pt1, 1});
      if (!success) {
        ++(it->second);
      }
      pt1.first += delta.first;
      pt1.second += delta.second;
    }
    /* And the final point.  */
    auto [it, success] = vents.insert({pt1, 1});
    if (!success) {
      ++(it->second);
    }
  }

  auto danger_count{
    std::accumulate(vents.begin(), vents.end(), unsigned{0},
                    [](auto a, auto const& it) { return a + (it.second >= 2 ? 1 : 0); })};

  std::cout << "Danger spots: " << danger_count << '\n';
  return 0;
}