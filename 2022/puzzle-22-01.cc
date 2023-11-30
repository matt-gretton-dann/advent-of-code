//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <cassert>
#include <array>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

using Point = std::pair<Int, Int>;
using Grid = std::vector<std::string>;

using namespace std::string_literals;

auto find_start(Grid const& grid) -> Point
{
  auto const& row0 = grid[0];
  for (auto it{row0.begin()}; it != row0.end(); ++it) {
    if (*it == '.') {
      return Point{it - row0.begin(), 0};
    }
  }
  std::abort();
}

auto turn_right(Point const& direction) -> Point
{
  return Point{-direction.second, direction.first};
}

auto turn_left(Point const& direction) -> Point
{
  return Point{direction.second, -direction.first};
}

auto read_num(std::string const& str, std::size_t ip) -> std::pair<Int, std::size_t>
{
  std::size_t pos{0};
  Int const num{std::stoll(str.substr(ip), &pos)};
  return {num, ip + pos};
}

auto move_one_step(Grid const& grid, Point const& pos, Point const& direction) -> Point
{
  bool again{true};
  Point result{pos};
  Int const grid_size{static_cast<Int>(grid.size())};
  while (again) {
    again = false;
    Point trial{result.first + direction.first, result.second + direction.second};

    // Handle wrap around.
    if (trial.second >= grid_size) {
      trial.second -= grid_size;
    }
    else if (trial.second < 0) {
      trial.second += grid_size;
    }
    auto const& row = grid.at(trial.second);
    Int const row_size{static_cast<Int>(row.size())};
    if (trial.first >= row_size) {
      trial.first -= row_size;
    }
    else if (trial.first < 0) {
      trial.first += row_size;
    }

    if (row[trial.first] == '#') {
      trial = pos;
    }
    result = trial;
    if (row[trial.first] == ' ') {
      again = true;
    }
  }

  return result;
}

auto direction_char(Point const& direction) -> char
{
  if (direction.first == 1) {
    assert(direction.second == 0);
    return '>';
  }
  if (direction.second == 1) {
    assert(direction.first == 0);
    return 'v';
  }
  if (direction.first == -1) {
    assert(direction.second == 0);
    return '<';
  }
  if (direction.second == -1) {
    assert(direction.first == 0);
    return '^';
  }

  std::abort();
}

auto move_pos(Grid& grid, Point pos, Point const& direction, Int amt) -> Point
{
  for (; amt > 0; --amt) {
    Point const next_pos{move_one_step(grid, pos, direction)};
    if (next_pos == pos) {
      break;
    }
    pos = next_pos;
    grid[pos.second][pos.first] = direction_char(direction);
  }
  return pos;
}

auto encode_direction(Point const& direction) -> Int
{
  if (direction.first == 1) {
    assert(direction.second == 0);
    return 0;
  }
  if (direction.second == 1) {
    assert(direction.first == 0);
    return 1;
  }
  if (direction.first == -1) {
    assert(direction.second == 0);
    return 2;
  }
  if (direction.second == -1) {
    assert(direction.first == 0);
    return 3;
  }

  std::abort();
}

auto main() -> int
{
  std::string line;
  Grid grid;

  std::size_t max_row_length{0};
  while (std::getline(std::cin, line) && !line.empty()) {
    grid.push_back(line);
    max_row_length = std::max(max_row_length, line.size());
  }
  for (auto& row : grid) { row.resize(max_row_length, ' '); };

  std::string instructions;
  if (!std::getline(std::cin, instructions)) {
    std::cerr << "Cannot read instructions.\n";
    return EXIT_FAILURE;
  }

  Point direction{1, 0};
  Point pos{find_start(grid)};

  std::size_t ip{0};
  while (ip < instructions.size()) {
    grid[pos.second][pos.first] = direction_char(direction);
    if (instructions[ip] == 'R') {
      std::cout << "R";
      direction = turn_right(direction);
      ++ip;
    }
    else if (instructions[ip] == 'L') {
      std::cout << "L";
      direction = turn_left(direction);
      ++ip;
    }
    else {
      auto [amt, new_ip] = read_num(instructions, ip);
      std::cout << amt;
      ip = new_ip;
      pos = move_pos(grid, pos, direction, amt);
    }
    Int const facing{encode_direction(direction)};

    std::cout << " Position: " << pos.first << ", " << pos.second << " Facing: " << direction.first
      << ", " << direction.second << " = " << facing << "\n";
  }

  Int const facing{encode_direction(direction)};
  pos.first += 1;
  pos.second += 1;
  for (auto const& row : grid) {
    std::cout << row << "\n";
  }
  std::cout << "Position: " << pos.first << ", " << pos.second << " Facing: " << direction.first
    << ", " << direction.second << " = " << facing << "\n";
  std::cout << "Encoding: " << pos.second * 1000 + pos.first * 4 + facing;

  return EXIT_SUCCESS;
}
