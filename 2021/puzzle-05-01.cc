//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>

using Point = std::pair<unsigned, unsigned>;

auto get_points(std::string const& s) -> std::pair<Point, Point>
{
  static std::regex re{R"((\d+),(\d+) -> (\d+),(\d+))"};
  std::smatch m;
  if (!std::regex_search(s, m, re)) {
    std::cerr << "Unable to interpret: " << s << "\n";
    std::exit(1);
  }
  return {{std::stoul(m.str(1)), std::stoul(m.str(2))},
          {std::stoul(m.str(3)), std::stoul(m.str(4))}};
}

auto main() -> int
{
  std::string line;
  std::map<Point, unsigned> vents;
  while (std::getline(std::cin, line)) {
    auto [pt1, pt2] = get_points(line);
    std::cout << "{" << pt1.first << ", " << pt1.second << "} -> {" << pt2.first << ", "
              << pt2.second << "}: ";
    if (pt1.first == pt2.first) {
      unsigned y1{std::min(pt1.second, pt2.second)};
      unsigned y2{std::max(pt1.second, pt2.second)};
      std::cout << "vertical:\n";
      while (y1 <= y2) {
        auto [it, success] = vents.insert({{pt1.first, y1}, 1});
        if (!success) {
          ++(it->second);
        }
        ++y1;
      }
    }
    else if (pt1.second == pt2.second) {
      unsigned x1{std::min(pt1.first, pt2.first)};
      unsigned x2{std::max(pt1.first, pt2.first)};
      std::cout << "horizontal:\n";
      while (x1 <= x2) {
        auto [it, success] = vents.insert({{x1, pt1.second}, 1});
        if (!success) {
          ++(it->second);
        }
        ++x1;
      }
    }
    else {
      std::cout << "sloped\n";
    }
  }

  auto danger_count{
    std::accumulate(vents.begin(), vents.end(), unsigned{0},
                    [](auto a, auto const& it) { return a + (it.second >= 2 ? 1 : 0); })};

  std::cout << "Danger spots: " << danger_count << '\n';
  return 0;
}