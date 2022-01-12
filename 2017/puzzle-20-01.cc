#include <iostream>
#include <limits>
#include <regex>
#include <string>

auto manhatten_distance(long x, long y, long z) { return std::abs(x) + std::abs(y) + std::abs(z); }

auto main() -> int
{
  static std::regex re{"p=<(-?\\d+),(-?\\d+),(-?\\d+)>, v=<(-?\\d+),(-?\\d+),(-?\\d+)>, "
                       "a=<(-?\\d+),(-?\\d+),(-?\\d+)>"};

  unsigned closest{0};
  unsigned dist_min{std::numeric_limits<unsigned>::max()};
  unsigned ddist_min{std::numeric_limits<unsigned>::max()};
  unsigned d2dist_min{std::numeric_limits<unsigned>::max()};
  std::string line;
  unsigned i{0};

  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << '\n';
    }
    auto x{std::stol(m.str(1))};
    auto y{std::stol(m.str(2))};
    auto z{std::stol(m.str(3))};
    auto dx{std::stol(m.str(4))};
    auto dy{std::stol(m.str(5))};
    auto dz{std::stol(m.str(6))};
    auto d2x{std::stol(m.str(7))};
    auto d2y{std::stol(m.str(8))};
    auto d2z{std::stol(m.str(9))};

    auto dist{manhatten_distance(x, y, z)};
    auto ddist{manhatten_distance(dx, dy, dz)};
    auto d2dist{manhatten_distance(d2x, d2y, d2z)};
    if (d2dist < d2dist_min || (d2dist == d2dist_min && ddist < ddist_min) ||
        (d2dist == d2dist_min && ddist == ddist_min && dist < dist_min)) {
      d2dist_min = d2dist;
      ddist_min = ddist;
      dist_min = dist;
      closest = i;
    }
    ++i;
  }

  std::cout << "Closest: " << closest << '\n';
  return 0;
}
