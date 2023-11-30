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

auto wrap_round(Point const& pos, Point const& direction) -> std::pair<Point, Point>
{
  Int constexpr square_size{50};
  Point constexpr up{0, -1};
  Point constexpr right{1, 0};
  Point constexpr down{0, 1};
  Point constexpr left{-1, 0};
  Int const sx = pos.first < 0 ? -1 : pos.first / square_size;
  Int const sy = pos.second < 0 ? -1 : pos.second / square_size;
  Int const x = pos.first < 0 ? square_size + pos.first : pos.first % square_size;
  Int const y = pos.second < 0 ? square_size + pos.second : pos.second % square_size;
  assert(x >= 0);
  assert(x < square_size);
  assert(y >= 0);
  assert(y < square_size);
  assert(sx * square_size + x == pos.first);
  assert(sy * square_size + y == pos.second);

  // Square looks like:
  //   AB
  //   C
  //  DE
  //  F

  if (sx == 1 && sy == -1 && direction == up) {
    // A-top -> F-left
    return {Point{0, x + square_size * 3}, right};
  }
  if (sx == 2 && sy == -1 && direction == up) {
    // B-top -> F-bottom
    return {Point{x, square_size * 3 + square_size - 1}, up};
  }
  if (sx == 0 && sy == 0 && direction == left) {
    // A-left -> D-left, reversed
    return {Point{0, square_size * 2 + square_size - 1 - y}, right};
  }
  if (sx == 3 && sy == 0 && direction == right) {
    // B-right -> E-right, reversed
    return {Point{square_size + square_size - 1, square_size * 2 + square_size - 1 - y}, left};
  }
  if (sx == 2 && sy == 1 && direction == down) {
    // B-bottom -> C->right
    return {Point{square_size + square_size - 1, square_size + x}, left};
  }
  if (sx == 0 && sy == 1 && direction == left) {
    // C->left -> D-top
    return {Point{y, square_size * 2}, down};
  }
  if (sx == 2 && sy == 1 && direction == right) {
    // C-right -> B-bottom
    return {Point{square_size * 2 + y, square_size - 1}, up};
  }
  if (sx == 0 && sy == 1 && direction == up) {
    // D-top -> C-left
    return {Point{square_size, x + square_size}, right};
  }
  if (sx == -1 && sy == 2 && direction == left) {
    // D-left -> A-left, reversed
    return {Point{square_size, square_size - 1 - y}, right};
  }
  if (sx == 2 && sy == 2 && direction == right) {
    // E-right -> B-right, reversed
    return {Point{square_size * 2 + square_size - 1, square_size - 1 - y}, left};
  }
  if (sx == 1 && sy == 3 && direction == down) {
    // E-down -> F-right
    return {Point{square_size - 1, square_size * 3 + x}, left};
  }
  if (sx == -1 && sy == 3 && direction == left) {
    // F-left -> A-top
    return {Point{square_size + y, 0}, down};
  }
  if (sx == 1 && sy == 3 && direction == right) {
    // F-right -> E-bottom
    return {Point{square_size + y, square_size * 2 + square_size - 1}, up};
  }
  if (sx == 0 && sy == 4 && direction == down) {
    // F-bottom -> B-top
    return {Point{square_size * 2 + x, 0}, down};
  }
  assert(((sx == 1 || sx == 2) && sy == 0) || (sx == 1 && sy == 1) ||
    ((sx == 0 || sx == 1) && sy == 2) || (sx == 0 && sy == 3));
  return {pos, direction};
}

auto move_one_step(Grid const& grid, Point const& pos, Point const& direction)
  -> std::pair<Point, Point>
{
  Point const trial{pos.first + direction.first, pos.second + direction.second};
  auto [wrapped_trial, wrapped_direction] = wrap_round(trial, direction);
  assert(wrapped_trial.second >= 0);
  assert(wrapped_trial.second < grid.size());
  auto const& row = grid.at(wrapped_trial.second);
  assert(wrapped_trial.first >= 0);
  assert(wrapped_trial.first < row.size());
  assert(row.at(wrapped_trial.first) != ' ');

  if (row.at(wrapped_trial.first) == '#') {
    return {pos, direction};
  }
  return std::make_pair(wrapped_trial, wrapped_direction);
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

auto move_pos(Grid& grid, Point pos, Point direction, Int amt) -> std::pair<Point, Point>
{
  for (; amt > 0; --amt) {
    auto const [next_pos, next_direction] = move_one_step(grid, pos, direction);
    if (next_pos == pos) {
      break;
    }
    if (direction != next_direction) {
      std::cout << " Change of direction: " << pos.first << ", " << pos.second << " -> "
        << next_pos.first << ", " << next_pos.second << ".  Direction "
        << direction_char(direction) << " -> " << direction_char(next_direction) << "\n";
    }
    pos = next_pos;
    direction = next_direction;
    grid[pos.second][pos.first] = direction_char(direction);
  }
  return {pos, direction};
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
  for (auto& row : grid) {
    row.resize(max_row_length, ' ');
  }

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
      auto [new_pos, new_direction] = move_pos(grid, pos, direction, amt);
      pos = new_pos;
      direction = new_direction;
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
