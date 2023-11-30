//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <cassert>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <regex>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;
using Point3 = std::tuple<UInt, UInt, UInt>;

using namespace std::string_literals;

struct Grid3
{
  explicit Grid3(std::size_t size)
    : size_(size) { map_.resize(size * size * size, false); }

  auto point(Point3 const& p) const noexcept -> bool
  {
    assert(std::get<0>(p) < size_);
    assert(std::get<1>(p) < size_);
    assert(std::get<2>(p) < size_);
    return map_[std::get<0>(p) * size_ * size_ + std::get<1>(p) * size_ + std::get<2>(p)];
  }

  void point(Point3 const& p, bool v)
  {
    assert(std::get<0>(p) < size_);
    assert(std::get<1>(p) < size_);
    assert(std::get<2>(p) < size_);
    map_[std::get<0>(p) * size_ * size_ + std::get<1>(p) * size_ + std::get<2>(p)] = v;
  }

  std::size_t size_;
  std::vector<bool> map_;
};

auto main() -> int
{
  std::string line;
  std::vector<Point3> points;
  UInt max{0};
  std::regex const re{"(\\d+),(\\d+),(\\d+)"};

  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Cannot interpret: " << line << "\n";
      return EXIT_FAILURE;
    }
    Point3 pt{std::make_tuple(std::stoull(m.str(1)), std::stoull(m.str(2)), std::stoull(m.str(3)))};
    max = std::max(std::get<0>(pt), max);
    max = std::max(std::get<1>(pt), max);
    max = std::max(std::get<2>(pt), max);
    points.push_back(pt);
  }

  Grid3 grid(max + 2);

  for (auto const& pt : points) {
    grid.point(pt, true);
  }

  UInt exposed_sides{0};
  for (auto const& pt : points) {
    UInt x{std::get<0>(pt)};
    UInt y{std::get<1>(pt)};
    UInt z{std::get<2>(pt)};
    if (x > 0) {
      exposed_sides += grid.point(std::make_tuple(x - 1, y, z)) ? 0 : 1;
    }
    else {
      ++exposed_sides;
    }
    exposed_sides += grid.point(std::make_tuple(x + 1, y, z)) ? 0 : 1;
    if (y > 0) {
      exposed_sides += grid.point(std::make_tuple(x, y - 1, z)) ? 0 : 1;
    }
    else {
      ++exposed_sides;
    }
    exposed_sides += grid.point(std::make_tuple(x, y + 1, z)) ? 0 : 1;
    if (z > 0) {
      exposed_sides += grid.point(std::make_tuple(x, y, z - 1)) ? 0 : 1;
    }
    else {
      ++exposed_sides;
    }
    exposed_sides += grid.point(std::make_tuple(x, y, z + 1)) ? 0 : 1;
  }

  std::cout << "Surface area: " << exposed_sides << "\n";
#if 0
  for (UInt z{0}; z <= max; ++z) {
    std::cout << "\nz = " << z << "\n";
    for (UInt y{0}; y <= max; ++y) {
      for (UInt x{0}; x <= max; ++x) {
        std::cout << (grid.point(std::make_tuple(x, y, z)) ? '#' : ' ');
      }
      std::cout << "\n";
    }
  }
#endif
  return EXIT_SUCCESS;
}
