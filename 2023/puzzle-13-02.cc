#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using UInt = std::uint64_t;

auto has_smidge(std::string_view row1, std::string_view row2) -> bool
{
  assert(row1.size() == row2.size());
  UInt diff_count{0};
  for (auto it1{row1.begin()}, it2{row2.begin()}; it1 != row1.end(); ++it1, ++it2) {
    diff_count += static_cast<UInt>(*it1 != *it2);
  }
  return diff_count == 1;
}

struct Grid
{
  void clear() { rows_.clear(); }
  void push_back(std::string const& row) { rows_.push_back(row); }

  [[nodiscard]] auto reflection_point(UInt const skip_row = 0) const -> UInt
  {
    for (std::size_t row{0}; row < rows_.size() - 1; ++row) {
      if (row + 1 == skip_row) { continue; }
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

  [[nodiscard]] auto find_smidge() const -> UInt
  {
    auto const skip_row{reflection_point()};

    for (std::size_t row{0}; row < rows_.size() - 1; ++row) {
      for (std::size_t row2{row + 1}; row2 < rows_.size(); ++row2) {
        if (has_smidge(rows_[row], rows_[row2])) {
          Grid temp(*this);
          temp.rows_[row2] = temp.rows_[row];

          if (auto const reflection_point = temp.reflection_point(skip_row);
            reflection_point != 0) {
            std::cout << "Found smidge between rows " << row << " and " << row2 << ". Score: " <<
              reflection_point << " Skip row: " << skip_row << '\n';
            for (std::size_t i{0}; i < rows_.size(); ++i) {
              std::cout << rows_[i];
              if (i == row || i == row2) { std::cout << " <<<"; }
              std::cout << '\n';
            }
            return reflection_point;
          }
        }
      }
    }

    std::cout << "No smidge point.\n";
    return 0;
  }

  [[nodiscard]] auto rotate_right() const -> Grid
  {
    std::cout << "Rotate right\n";
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
      rows += grid.find_smidge();
      grid = grid.rotate_right();
      cols += grid.find_smidge();
      grid.clear();
      std::cout << "CLEAR\n";
    }
    else {
      grid.push_back(line);
    }
  }

  rows += grid.find_smidge();
  grid = grid.rotate_right();
  cols += grid.find_smidge();

  std::cout << "Columns to left: " << cols << '\n' << "Rows above: " << rows << '\n' << "Score: " <<
    cols + (rows * 100) << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
