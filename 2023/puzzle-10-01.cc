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

private:
  std::vector<std::string> grid_;
  Pos animal_{std::string::npos, std::string::npos};
};

auto main() -> int try {
  std::string line;
  Grid grid;

  while (std::getline(std::cin, line)) {
    grid.push_line(line);
  }

  auto animal = grid.animal_pos();

  if (animal.second > 0) {
    auto [end_pos, length] = grid.walk_pipes(animal, {animal.first, animal.second - 1});
    if (end_pos == animal) {
      std::cout << "Started going north ended back at animal: " << length / 2 + 1 << '\n';
    }
  }

  if (animal.first > 0) {
    auto [end_pos, length] = grid.walk_pipes(animal, {animal.first - 1, animal.second});
    if (end_pos == animal) {
      std::cout << "Started going west ended back at animal: " << length / 2 + 1 << '\n';
    }
  }

  if (animal.second < grid.height() - 1) {
    auto [end_pos, length] = grid.walk_pipes(animal, {animal.first, animal.second + 1});
    if (end_pos == animal) {
      std::cout << "Started going south ended back at animal: " << length / 2 + 1 << '\n';
    }
  }

  if (animal.first < grid.width() - 1) {
    auto [end_pos, length] = grid.walk_pipes(animal, {animal.first + 1, animal.second});
    if (end_pos == animal) {
      std::cout << "Started going east ended back at animal: " << length / 2 + 1 << '\n';
    }
  }

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
