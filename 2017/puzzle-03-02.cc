#include <iostream>
#include <map>
#include <string>

using Int = long;
using Coord = std::pair<Int, Int>;
using ValueMap = std::map<Coord, Int>;

template<typename T>
auto sign(T i) -> decltype(i)
{
  if (i < 0) {
    return -1;
  }
  if (i > 0) {
    return 1;
  }
  return 0;
}

auto value(ValueMap const& map, Coord const& coord) -> Int
{
  auto result{0};
  for (auto x{coord.first - 1}; x < coord.first + 2; ++x) {
    for (auto y{coord.second - 1}; y < coord.second + 2; ++y) {
      if (x == coord.first && y == coord.second) {
        continue;
      }
      auto it{map.find(Coord{x, y})};
      if (it != map.end()) {
        result += it->second;
      }
    }
  }

  return result;
}

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Broken line" << '\n';
    return 1;
  }

  auto target{std::stol(line)};
  Coord dir{1, 0};
  Coord pos{0, 0};
  Int count{1};
  Int count_reset{1};
  ValueMap map;
  map.insert({pos, 1});

  while (true) {
    while (count-- != 0) {
      pos = {pos.first + dir.first, pos.second + dir.second};
      auto val{value(map, pos)};
      if (val > target) {
        std::cout << val << '\n';
        return 0;
      }
      map.insert({pos, val});
    }
    dir = {-dir.second, dir.first};
    if (dir.first != 0) {
      ++count_reset;
    }
    count = count_reset;
  }
}
