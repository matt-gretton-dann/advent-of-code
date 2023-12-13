#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using UInt = std::uint64_t;

struct Grid
{
  void clear() { rows_.clear(); }
  void push_back(std::string const& row) { rows_.push_back(row); }

  [[nodiscard]] auto reflection_point() const -> UInt
  {
    for (std::size_t row{0}; row < rows_.size() - 1; ++row) {
      auto top{row};
      auto bottom{row + 1};
      while (rows_[top] == rows_[bottom]) {
        if (top == 0 || bottom == rows_.size() - 1) { return row + 1; }
        --top;
        ++bottom;
      }
    }

    return 0;
  }

  [[nodiscard]] auto rotate_right() const -> Grid
  {
    Grid new_grid;
    new_grid.rows_.resize(rows_.front().size());
    for (auto row{rows_.rbegin()}; row != rows_.rend(); ++row) {
      for (std::size_t i{0}; i < row->size(); ++i) {
        new_grid.rows_[i].push_back(row->at(i));
      }
    }

    return new_grid;
  }

private:
  std::vector<std::string> rows_;
};

auto main() -> int try {
  std::string line;

  Grid grid;
  UInt rows{0};
  UInt cols{0};
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      rows += grid.reflection_point();
      grid = grid.rotate_right();
      cols += grid.reflection_point();
      grid.clear();
    }
    else { grid.push_back(line); }
  }

  rows += grid.reflection_point();
  grid = grid.rotate_right();
  cols += grid.reflection_point();

  std::cout << "Columns to left: " << cols << '\n' << "Rows above: " << rows << '\n' << "Score: " <<
    cols + (rows * 100) << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
