//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>

using UInt = std::uint64_t;
using Point = std::pair<UInt, UInt>;

auto contained_in(Point const& super, Point const& sub) noexcept -> bool
{
  return super.first <= sub.first && super.second >= sub.second;
}

auto overlap(Point const& l, Point const& r) noexcept -> bool
{
  //  aaaa --- bbbb
  //  aaCCbb
  //  bbCCaa
  //  bbbb --  aaaa
  return l.second >= r.first && l.first <= r.second;
}

auto get_points(std::string const& s) -> std::pair<Point, Point>
{
  static std::regex re{R"((\d+)-(\d+),(\d+)-(\d+))"};
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
  UInt count{0};
  while (std::getline(std::cin, line)) {
    auto [pt1, pt2] = get_points(line);
    if (overlap(pt2, pt1)) {
      ++count;
    }
  }

  std::cout << "Completely duplicated effort: " << count << '\n';
  return 0;
}
