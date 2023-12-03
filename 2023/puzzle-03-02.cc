#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using UInt = std::uint64_t;

struct Grid
{
  void push(std::string const& s)
  {
    if (grid_.empty()) {
      grid_.emplace_back(s.length() + 2, '.');
      grid_.emplace_back(s.length() + 2, '.');
    }

    assert(s.length() + 2 == grid_.front().length());
    grid_.emplace(grid_.end() - 1, std::string(".") + s + std::string("."));
  }

  [[nodiscard]] auto loc(std::size_t const y, std::size_t const x) const -> UInt
  {
    return y * grid_.front().size() + x;
  }

  [[nodiscard]] auto sum_parts() const -> UInt
  {
    std::map<UInt, std::vector<UInt>> parts;

    for (std::size_t y{1}; y < grid_.size(); ++y) {
      for (auto x{1}; x < grid_[y].size(); ++x) {
        if (std::isdigit(grid_[y][x]) != 0) {
          std::vector<UInt> stars;
          UInt value = 0;
          if (grid_[y - 1][x - 1] == '*') { stars.emplace_back(loc(y - 1, x - 1)); }
          if (grid_[y][x - 1] == '*') { stars.emplace_back(loc(y, x - 1)); }
          if (grid_[y + 1][x - 1] == '*') { stars.emplace_back(loc(y + 1, x - 1)); }
          while (std::isdigit(grid_[y][x]) != 0) {
            value *= 10;
            value += grid_[y][x] - '0';
            if (grid_[y - 1][x] == '*') { stars.emplace_back(loc(y - 1, x)); }
            if (grid_[y + 1][x] == '*') { stars.emplace_back(loc(y + 1, x)); }
            ++x;
          }
          if (grid_[y - 1][x] == '*') { stars.emplace_back(loc(y - 1, x)); }
          if (grid_[y][x] == '*') { stars.emplace_back(loc(y, x)); }
          if (grid_[y + 1][x] == '*') { stars.emplace_back(loc(y + 1, x)); }

          for (auto star : stars) {
            auto [it, inserted] = parts.insert(std::make_pair(star, std::vector<UInt>()));
            it->second.push_back(value);
          }
        }
      }
    }

    UInt sum{0};
    for (auto const& [id, sums] : parts) {
      if (sums.size() == 2) {
        sum += sums[0] * sums[1];
      }
    }

    return sum;
  }

  std::vector<std::string> grid_;
};

auto main() -> int try {
  Grid grid;

  std::string line;
  while (std::getline(std::cin, line)) {
    grid.push(line);
  }

  std::cout << "Part sum: " << grid.sum_parts() << '\n';
  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
