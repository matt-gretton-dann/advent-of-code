//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <cassert>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

using Point = std::pair<Int, Int>;
using Grid = std::vector<std::string>;
using Dirs = std::list<std::array<Point, 3>>;

constexpr Point north{0, -1};
constexpr Point north_east{1, -1};
constexpr Point east{1, 0};
constexpr Point south_east{1, 1};
constexpr Point south{0, 1};
constexpr Point south_west{-1, 1};
constexpr Point west{-1, 0};
constexpr Point north_west{-1, -1};

auto next_state(Grid const& grid, Dirs const& dirs) -> Grid
{
  constexpr std::array<Point, 8> all_dirs{north, north_east, east, south_east,
                                          south, south_west, west, north_west};

  UInt const width{grid.at(0).size()};
  UInt const height{grid.size()};
  Int num_pts{0};
  Grid result(height, std::string(width, '.'));
  // We have designed the grid so we shouldn't need to check the edges.
  for (unsigned y{1}; y < height - 1; ++y) {
    for (unsigned x{1}; x < width - 1; ++x) {
      if (grid.at(y).at(x) == '.') {
        continue;
      }
      ++num_pts;

      if (std::all_of(all_dirs.begin(), all_dirs.end(), [&x, &y, &grid](auto const& dir) {
        return grid.at(dir.second + y).at(dir.first + x) == '.';
      })) {
        result.at(y).at(x) = '#';
        continue;
      }

      bool moved{false};
      Point direction{0, 0};
      for (auto const& ds : dirs) {
        if (std::all_of(ds.begin(), ds.end(), [&x, &y, &grid](auto const& d) {
          return grid.at(y + d.second).at(x + d.first) == '.';
        })) {
          moved = true;
          direction = ds[1];
        }
        if (moved) {
          break;
        }
      }

      char const coord{result.at(y + direction.second).at(x + direction.first)};
      if (result.at(y + direction.second).at(x + direction.first) == '.') {
        result.at(y + direction.second).at(x + direction.first) =
          static_cast<char>('#' + (direction.second * 4) + direction.first);
      }
      else {
        result.at(y).at(x) = '#';
        int dx{0};
        int dy{0};
        if (coord != 'M') {
          switch (coord) {
          case '#' - 4 - 1:
            dx = 1;
            dy = 1;
            break;
          case '#' - 4:
            dx = 0;
            dy = 1;
            break;
          case '#' - 4 + 1:
            dx = -1;
            dy = 1;
            break;
          case '#' - 1:
            dx = 1;
            dy = 0;
            break;
          case '#' + 1:
            dx = -1;
            dy = 0;
            break;
          case '#' + 4 - 1:
            dx = 1;
            dy = -1;
            break;
          case '#' + 4:
            dx = 0;
            dy = -1;
            break;
          case '#' + 4 + 1:
            dx = -1;
            dy = -1;
            break;
          default:
            abort();
          }
          result.at(y + direction.second).at(x + direction.first) = 'M';
          result.at(y + direction.second + dy).at(x + direction.first + dx) = '#';
        }
      }
    }
  }

  for (unsigned y{0}; y < height; ++y) {
    for (unsigned x{0}; x < width; ++x) {
      if (result.at(y).at(x) == 'M') {
        result.at(y).at(x) = '.';
      }
      else if (result.at(y).at(x) != '.') {
        result.at(y).at(x) = '#';
        --num_pts;
      }
    }
  }

  assert(num_pts == 0);
  return result;
}

auto calc_score(Grid const& grid) -> UInt
{
  UInt const width{grid.at(0).size()};
  UInt const height{grid.size()};
  UInt num_pts{0};
  UInt min_x{std::numeric_limits<UInt>::max()};
  UInt min_y{std::numeric_limits<UInt>::max()};
  UInt max_x{0};
  UInt max_y{0};

  for (UInt y{0}; y < height; ++y) {
    for (UInt x{0}; x < width; ++x) {
      if (grid[y][x] == '#') {
        min_x = std::min(min_x, x);
        min_y = std::min(min_y, y);
        max_x = std::max(max_x, x);
        max_y = std::max(max_y, y);
        ++num_pts;
      }
    }
  }

  return (max_x - min_x + 1) * (max_y - min_y + 1) - num_pts;
}

using namespace std::string_literals;

auto main() -> int
{
  std::string line;
  Grid grid;

  Dirs dirs_to_consider = {{north_west, north, north_east},
                           {south_east, south, south_west},
                           {south_west, west, north_west},
                           {north_east, east, south_east}};

  // We surround each line of the grid with extra dots so we don't have to resize as we go along.
  std::size_t width{0};
  while (std::getline(std::cin, line)) {
    grid.push_back(line);
    width = std::max(width, line.size());
  }
  for (auto& s : grid) {
    s.resize(width, '.');
  }

  Grid last_grid;
  UInt num_rounds{0};
  while (grid != last_grid) {
#if 0
    std::cout << "\n\nStep " << num_rounds << "\n";
    for (auto const& r : grid) {
      std::cout << r << "\n";
    }
#endif
    ++num_rounds;
    for (auto& r : grid) {
      r.insert(r.begin(), '.');
      r.insert(r.end(), '.');
    }
    width += 2;
    grid.insert(grid.begin(), std::string(width, '.'));
    grid.insert(grid.end(), std::string(width, '.'));
    last_grid = grid;

    grid = next_state(grid, dirs_to_consider);
    dirs_to_consider.push_back(dirs_to_consider.front());
    dirs_to_consider.pop_front();
  }

  UInt const score{calc_score(grid)};
  std::cout << "Score: " << score << "\n";
  std::cout << "Number of turns: " << num_rounds << "\n";
  return EXIT_SUCCESS;
}
