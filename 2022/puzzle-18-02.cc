//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

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
  explicit Grid3(std::size_t size) : size_(size) { map_.resize(size * size * size, ' '); }

  auto point(Point3 const& p) const noexcept -> char
  {
    assert(std::get<0>(p) < size_);
    assert(std::get<1>(p) < size_);
    assert(std::get<2>(p) < size_);
    return map_[std::get<0>(p) * size_ * size_ + std::get<1>(p) * size_ + std::get<2>(p)];
  }

  void point(Point3 const& p, char v)
  {
    assert(std::get<0>(p) < size_);
    assert(std::get<1>(p) < size_);
    assert(std::get<2>(p) < size_);
    map_[std::get<0>(p) * size_ * size_ + std::get<1>(p) * size_ + std::get<2>(p)] = v;
  }

  std::size_t size_;
  std::vector<char> map_;
};

auto main() -> int
{
  std::string line;
  std::vector<Point3> points;
  UInt max{0};
  std::regex const re{"(\\d+),(\\d+),(\\d+)"};
  char const air{' '};
  char const water{'.'};
  char const rock{'#'};

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

  // Populate points
  for (auto const& pt : points) {
    grid.point(pt, rock);
  }

  // Now fill the grid with water
  std::vector<Point3> current_points;
  std::vector<Point3> next_points;
  next_points.push_back(std::make_tuple(0, 0, 0));
  while (!next_points.empty()) {
    std::vector<Point3> current_points(std::move(next_points));
    next_points.clear();
    for (auto const& pt : current_points) {
      if (grid.point(pt) == air) {
        grid.point(pt, water);
        UInt x{std::get<0>(pt)};
        UInt y{std::get<1>(pt)};
        UInt z{std::get<2>(pt)};
        if (x > 0) {
          next_points.push_back(std::make_tuple(x - 1, y, z));
        }
        if (x < grid.size_ - 1) {
          next_points.push_back(std::make_tuple(x + 1, y, z));
        }
        if (y > 0) {
          next_points.push_back(std::make_tuple(x, y - 1, z));
        }
        if (y < grid.size_ - 1) {
          next_points.push_back(std::make_tuple(x, y + 1, z));
        }
        if (z > 0) {
          next_points.push_back(std::make_tuple(x, y, z - 1));
        }
        if (z < grid.size_ - 1) {
          next_points.push_back(std::make_tuple(x, y, z + 1));
        }
      }
    }
  }

  UInt exposed_sides{0};
  for (auto const& pt : points) {
    UInt x{std::get<0>(pt)};
    UInt y{std::get<1>(pt)};
    UInt z{std::get<2>(pt)};
    if (x > 0) {
      exposed_sides += grid.point(std::make_tuple(x - 1, y, z)) == water ? 1 : 0;
    }
    else {
      ++exposed_sides;
    }
    exposed_sides += grid.point(std::make_tuple(x + 1, y, z)) == water ? 1 : 0;
    if (y > 0) {
      exposed_sides += grid.point(std::make_tuple(x, y - 1, z)) == water ? 1 : 0;
    }
    else {
      ++exposed_sides;
    }
    exposed_sides += grid.point(std::make_tuple(x, y + 1, z)) == water ? 1 : 0;
    if (z > 0) {
      exposed_sides += grid.point(std::make_tuple(x, y, z - 1)) == water ? 1 : 0;
    }
    else {
      ++exposed_sides;
    }
    exposed_sides += grid.point(std::make_tuple(x, y, z + 1)) == water ? 1 : 0;
  }

  std::cout << "Surface area: " << exposed_sides << "\n";
  for (UInt z{0}; z <= max; ++z) {
    std::cout << "\nz = " << z << "\n";
    for (UInt y{0}; y <= max; ++y) {
      for (UInt x{0}; x <= max; ++x) {
        std::cout << grid.point(std::make_tuple(x, y, z));
      }
      std::cout << "\n";
    }
  }
  return EXIT_SUCCESS;
}
