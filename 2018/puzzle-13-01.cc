//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <regex>
#include <set>
#include <utility>

using Int = std::int64_t;
using UInt = std::uint64_t;
using Point = std::pair<Int, Int>;

enum DirSelect {
  left = 0,
  straight = 1,
  right = 2,
};

constexpr Point north{0, -1};
constexpr Point east{1, 0};
constexpr Point south{0, 1};
constexpr Point west{-1, 0};

struct Truck
{
  Point pt_{0, 0};
  Point dir_{north};
  std::size_t dir_select_{0};
  UInt time_stamp_{0};

  constexpr auto operator<(Truck const& r) const noexcept -> bool
  {
    return pt_.first < r.pt_.first || (pt_.first == r.pt_.first && pt_.second < r.pt_.second);
  }
};

using State = std::set<Truck>;

auto main() -> int
{
  std::string line;
  std::vector<std::string> map;
  State trucks;

  Point pos{0, 0};
  while (std::getline(std::cin, line)) {
    for (auto& c : line) {
      switch (c) {
      case '>':
        trucks.insert({pos, east, 0, 0});
        c = '-';
        break;
      case '<':
        trucks.insert({pos, west, 0, 0});
        c = '-';
        break;
      case '^':
        trucks.insert({pos, north, 0, 0});
        c = '|';
        break;
      case 'v':
        trucks.insert({pos, south, 0, 0});
        c = '|';
        break;
      }
      pos.first += 1;
    }
    map.push_back(line);
    pos.first = 0;
    pos.second += 1;
  }

  UInt time_step{0};
  while (true) {
    ++time_step;
    std::cout << "\n" << std::setw(3) << time_step << ": ";
    auto truck{trucks.begin()};
    while (truck != trucks.end()) {
      if (truck->time_stamp_ >= time_step) {
        ++truck;
        continue;
      }
      std::cout << truck->pt_.first << "," << truck->pt_.second << ", ";
      Point dir{truck->dir_};
      std::size_t dir_select = truck->dir_select_;
      Point const pos{truck->pt_.first + dir.first, truck->pt_.second + dir.second};
      auto c{map.at(pos.second).at(pos.first)};
      switch (c) {
      case '|':
      case '-':
        break;
      case '/':
        if (dir == west) {
          dir = south;
        }
        else if (dir == south) {
          dir = west;
        }
        else if (dir == east) {
          dir = north;
        }
        else if (dir == north) {
          dir = east;
        }
        break;
      case '\\':
        if (dir == west) {
          dir = north;
        }
        else if (dir == north) {
          dir = west;
        }
        else if (dir == east) {
          dir = south;
        }
        else if (dir == south) {
          dir = east;
        }
        break;
      case '+':
        if (dir_select == left) {
          dir = Point{dir.second, -dir.first};
        }
        else if (dir_select == right) {
          dir = Point{-dir.second, dir.first};
        }
        dir_select = (dir_select + 1) % 3;
        break;
      default:
        abort();
      }
      Truck const next_truck{pos, dir, dir_select, time_step};
      truck = trucks.erase(truck);
      auto [next_it, success] = trucks.insert(next_truck);
      if (!success) {
        std::cout << "Crash at: " << pos.first << "," << pos.second << "\n";
        return EXIT_SUCCESS;
      }
    }
  }
}
