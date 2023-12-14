#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using UInt = std::uint64_t;

struct Grid
{
  void push_back(std::string const& row) { rows_.push_back(row); }

  void roll_north()
  {
    for (auto row{rows_.begin() + 1}; row != rows_.end(); ++row) {
      for (std::size_t col{0}; col < row->size(); ++col) {
        if (row->at(col) == 'O') {
          auto new_row = row - 1;
          while (new_row != rows_.begin() && new_row->at(col) == '.') { --new_row; }
          if (new_row->at(col) != '.') { ++new_row; }
          if (new_row != row) {
            new_row->at(col) = 'O';
            row->at(col) = '.';
          }
        }
      }
    }
  }

  [[nodiscard]] auto load() const -> UInt
  {
    UInt load{0};
    for (std::size_t row{0}; row != rows_.size(); ++row) {
      auto count = std::count_if(rows_[row].begin(), rows_[row].end(),
                                 [](char c) { return c == 'O'; });
      load += count * (rows_.size() - row);
    }
    return load;
  }

  friend auto operator<<(std::ostream& os, Grid const& grid) -> std::ostream&;

private:
  std::vector<std::string> rows_;
};

auto operator<<(std::ostream& os, Grid const& grid) -> std::ostream&
{
  for (auto const& row : grid.rows_) { os << row << '\n'; }
  return os;
}

auto main() -> int try {
  std::string line;

  Grid grid;
  while (std::getline(std::cin, line)) {
    grid.push_back(line);
  }

  std::cout << "Read in grid:\n" << grid;
  grid.roll_north();
  std::cout << "Rolled grid:\n" << grid;
  std::cout << "Load: " << grid.load() << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
