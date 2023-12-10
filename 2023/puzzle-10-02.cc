#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using Int = std::int64_t;
using NumVec = std::vector<Int>;
using Pos = std::pair<std::size_t, std::size_t>; // (x, y) pair

[[nodiscard]] constexpr auto operator==(Pos const& lhs, Pos const& rhs) -> bool
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

struct Grid;
auto operator<<(std::ostream& os, Grid const& grid) -> std::ostream&;

struct Grid
{
  void push_line(std::string const& line)
  {
    if (auto x{line.find('S')}; x != std::string::npos) {
      assert(animal_.first == std::string::npos);
      animal_.first = x;
      animal_.second = grid_.size();
    }
    grid_.push_back(line);
  }

  [[nodiscard]] auto animal_pos() const -> Pos { return animal_; }

  [[nodiscard]] auto width() const -> std::size_t { return grid_.front().size(); }
  [[nodiscard]] auto height() const -> std::size_t { return grid_.size(); }

  [[nodiscard]] auto at(Pos const& pos) const -> char { return grid_[pos.second][pos.first]; }
  void set(Pos const& pos, char c) { grid_[pos.second][pos.first] = c; }

  [[nodiscard]] auto walk_pipes(Pos const& start, Pos pos) const -> std::pair<Pos, std::size_t>
  {
    auto last{start};
    std::size_t steps{0};
    while (true) {
      auto const [x, y] = pos;
      auto const c{grid_[y][x]};
      if (c == '.' || c == 'S') { break; }
      if (start == pos) { break; }

      if (c == '|' || c == 'L' || c == 'J') {
        if (Pos const next_pos{x, y - 1}; y > 0 && next_pos != last) {
          last = pos;
          pos = next_pos;
          ++steps;
          continue;
        }
      }

      if (c == '|' || c == 'F' || c == '7') {
        if (Pos const next_pos{x, y + 1}; y < height() - 1 && next_pos != last) {
          last = pos;
          pos = next_pos;
          ++steps;
          continue;
        }
      }

      if (c == '-' || c == '7' || c == 'J') {
        if (Pos const next_pos{x - 1, y}; x > 0 && next_pos != last) {
          last = pos;
          pos = next_pos;
          ++steps;
          continue;
        }
      }

      if (c == '-' || c == 'L' || c == 'F') {
        if (Pos const next_pos{x + 1, y}; x < width() - 1 && next_pos != last) {
          last = pos;
          pos = next_pos;
          ++steps;
          continue;
        }
      }

      std::abort();
    }

    return std::make_pair(pos, steps);
  }

  void hightlight_pipes(Pos const& start)
  {
    auto last{start};
    auto pos{start};

    auto const c{at(start)};
    if (c == '|' || c == 'L' || c == 'J') { pos.second -= 1; }
    else if (c == 'F' || c == '7') { pos.second += 1; }
    else if (c == '-') { pos.first += 1; }
    else { std::abort(); }
    set(start, c == '|' ? 'v' : '>');

    while (start != pos) {
      auto const [x, y] = pos;
      auto const c{grid_[y][x]};
      if (c == '.') { std::abort(); }
      grid_[y][x] = c == '|' ? 'v' : '>';

      if (c == '|' || c == 'L' || c == 'J') {
        if (Pos const next_pos{x, y - 1}; y > 0 && next_pos != last) {
          last = pos;
          pos = next_pos;
          continue;
        }
      }

      if (c == '|' || c == 'F' || c == '7') {
        if (Pos const next_pos{x, y + 1}; y < height() - 1 && next_pos != last) {
          last = pos;
          pos = next_pos;
          continue;
        }
      }

      if (c == '-' || c == '7' || c == 'J') {
        if (Pos const next_pos{x - 1, y}; x > 0 && next_pos != last) {
          last = pos;
          pos = next_pos;
          continue;
        }
      }

      if (c == '-' || c == 'L' || c == 'F') {
        if (Pos const next_pos{x + 1, y}; x < width() - 1 && next_pos != last) {
          last = pos;
          pos = next_pos;
          continue;
        }
      }

      std::abort();
    }
  }

  auto flood_fill()
  {
    /* Ensure every character is either an edge, or a '.' for outside, maybe inside, and a '*'
     * for the initial starting locations.
     */
    for (auto& row : grid_) {
      /* We know we're inside if we've passed an odd number of vertical walls and no corners/
       * horizontal walls.
       */
      bool inside = false;
      bool definite = true;
      for (auto& c : row) {
        if (c == 'v') { inside = !inside; }
        else if (c == '>') { definite = false; }
        else if (inside && definite) {
          c = '*';
        }
        else { c = '.'; }
      }
    }

    /* Very simple algorithm, that just iterates over all squares until we don't change anything */
    bool again{true};
    while (again) {
      again = false;
      for (std::size_t y{0}; y != height(); ++y) {
        for (std::size_t x{0}; x != width(); ++x) {
          if (grid_[y][x] != '.') { continue; }
          if ((y > 0 && grid_[y - 1][x] == '*') || (x > 0 && grid_[y][x - 1] == '*') || (
                y < height() - 1 && grid_[y + 1][x] == '*') || (
                x < width() - 1 && grid_[y][x + 1] == '*')) {
            grid_[y][x] = '*';
            again = true;
          }
        }
      }
    }
  }

  /* Shrink the grid by a factor of two in all directions.  We take the even places to be the new
   * entry in the new grid.
   *
   * So grid.enlarge().shrink() should produce the same grid as grid.
   */
  [[nodiscard]] auto shrink() const -> Grid
  {
    Grid new_grid;
    for (std::size_t y{0}; y < height(); y += 2) {
      std::string line;
      for (std::size_t x{0}; x < width(); x += 2) {
        line += grid_[y][x];
      }
      new_grid.push_line(line);
    }
    return new_grid;
  }

  /* Enlarge the grid by a factor of two in each direction.
   *
   * The contents of grid_[y][x] are mapped to grid_[y * 2][x * 2] and the appropriate additional
   * characters are added.
   */
  [[nodiscard]] auto enlarge() const -> Grid
  {
    Grid new_grid;
    for (auto const& row : grid_) {
      std::string line1;
      std::string line2;
      for (auto const c : row) {
        if (c == '.') {
          line1 += "..";
          line2 += "..";
        }
        if (c == '-') {
          line1 += "--";
          line2 += "..";
        }
        if (c == '7') {
          line1 += "7.";
          line2 += "|.";
        }
        if (c == 'F') {
          line1 += "F-";
          line2 += "|.";
        }
        if (c == '|') {
          line1 += "|.";
          line2 += "|.";
        }
        if (c == 'J') {
          line1 += "J.";
          line2 += "..";
        }
        if (c == 'L') {
          line1 += "L-";
          line2 += "..";
        }
      }
      new_grid.push_line(line1);
      new_grid.push_line(line2);
    }

    return new_grid;
  }

  [[nodiscard]] auto count_interior() const -> std::size_t
  {
    std::size_t interior{0};
    for (auto const& row : grid_) {
      interior += std::ranges::count_if(row, [](char const c) { return c == '*'; });
    }

    return interior;
  }

  friend auto operator<<(std::ostream& os, Grid const& grid) -> std::ostream&;

private:
  std::vector<std::string> grid_;
  Pos animal_{std::string::npos, std::string::npos};
};

auto operator<<(std::ostream& os, Grid const& grid) -> std::ostream&
{
  for (auto const& row : grid.grid_) {
    os << row << '\n';
  }
  return os;
}

auto main() -> int try {
  std::string line;
  Grid grid;

  while (std::getline(std::cin, line)) {
    grid.push_line(line);
  }

  auto animal = grid.animal_pos();

  /* Find the loop in both directions as this tells us what the animal pipe piece should be. */
  bool east{false};
  bool south{false};
  bool west{false};
  bool north{false};
  if (animal.second > 0) {
    Pos const next_pos{animal.first, animal.second - 1};
    if (auto const c{grid.at(next_pos)}; c == '|' || c == 'F' || c == '7') {
      auto [end_pos, length] = grid.walk_pipes(animal, next_pos);
      if (end_pos == animal) {
        std::cout << "Started going north ended back at animal: " << length / 2 + 1 << '\n';
        north = true;
      }
    }
  }

  if (animal.first > 0) {
    Pos const next_pos{animal.first - 1, animal.second};
    if (auto const c{grid.at(next_pos)}; c == '-' || c == 'F' || c == 'L') {
      auto [end_pos, length] = grid.walk_pipes(animal, next_pos);
      if (end_pos == animal) {
        std::cout << "Started going west ended back at animal: " << length / 2 + 1 << '\n';
        west = true;
      }
    }
  }

  if (animal.second < grid.height() - 1) {
    Pos const next_pos{animal.first, animal.second + 1};
    if (auto const c{grid.at(next_pos)}; c == '|' || c == 'J' || c == 'L') {
      auto [end_pos, length] = grid.walk_pipes(animal, next_pos);
      if (end_pos == animal) {
        std::cout << "Started going south ended back at animal: " << length / 2 + 1 << '\n';
        south = true;
      }
    }
  }

  if (animal.first < grid.width() - 1) {
    Pos const next_pos{animal.first + 1, animal.second};
    if (auto const c{grid.at(next_pos)}; c == '-' || c == 'J' || c == '7') {
      auto [end_pos, length] = grid.walk_pipes(animal, next_pos);
      if (end_pos == animal) {
        std::cout << "Started going east ended back at animal: " << length / 2 + 1 << '\n';
        east = true;
      }
    }
  }

  /* Replace animal with pipe piece. */
  if (north && south) { grid.set(animal, '|'); }
  else if (east && west) { grid.set(animal, '-'); }
  else if (north && east) { grid.set(animal, 'L'); }
  else if (north && west) { grid.set(animal, 'J'); }
  else if (south && east) { grid.set(animal, 'F'); }
  else if (south && west) { grid.set(animal, '7'); }
  else {
    std::abort();
  }

  /* To ensure that the interior is stongly connected we double the size of the grid in both
   * directions.  This makes sure that there is always a . between two pipe edges.
   *
   * This then means we can do a very simple flood fill to find the interior.
   *
   * We then shrink the grid again and to get the interior count we want.
   */
  auto big_grid{grid.enlarge()};
  // Mark the pipes we want to work with using > & v characters
  big_grid.hightlight_pipes(Pos{animal.first * 2, animal.second * 2});
  std::cout << "Before flood fill:\n" << big_grid;
  // Flood fill the interior with * characters
  big_grid.flood_fill();
  std::cout << "After flood fill:\n" << big_grid;
  // Shrink
  auto const small_grid = big_grid.shrink();
  // Count the number of * in the interior.
  auto const interior_count = small_grid.count_interior();
  std::cout << "Reconstituted grid\n" << small_grid;
  std::cout << "Interior count: " << interior_count << "\n";

  return EXIT_SUCCESS;;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
