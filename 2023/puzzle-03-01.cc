#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using UInt = std::uint64_t;

constexpr auto is_digit_or_dot(char c) -> bool
{
  return (std::isdigit(c) != 0) || c == '.';
}

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

  [[nodiscard]] auto sum_parts() const -> UInt
  {
    UInt sum{0};
    for (auto y{grid_.begin() + 1}; y != grid_.end(); ++y) {
      for (auto x{y->begin() + 1}; x != y->end(); ++x) {
        if (std::isdigit(*x) != 0) {
          bool is_connected{false};
          auto pos = x - y->begin();
          UInt value = 0;
          if (!is_digit_or_dot((y - 1)->at(pos - 1))) { is_connected = true; }
          if (!is_digit_or_dot((y)->at(pos - 1))) { is_connected = true; }
          if (!is_digit_or_dot((y + 1)->at(pos - 1))) { is_connected = true; }
          while (std::isdigit(*x) != 0) {
            value *= 10;
            value += *x - '0';
            if (!is_digit_or_dot((y - 1)->at(pos))) { is_connected = true; }
            if (!is_digit_or_dot((y + 1)->at(pos))) { is_connected = true; }
            ++x;
            ++pos;
          }
          if (!is_digit_or_dot((y - 1)->at(pos))) { is_connected = true; }
          if (!is_digit_or_dot((y)->at(pos))) { is_connected = true; }
          if (!is_digit_or_dot((y + 1)->at(pos))) { is_connected = true; }
          if (is_connected) { sum += value; }
        }
      }
    }

    return sum;
  }

  std::vector<std::string> grid_;
};

auto main() -> int try
{
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
