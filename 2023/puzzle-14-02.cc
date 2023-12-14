#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

using UInt = std::uint64_t;

struct Grid
{
  friend auto operator<<(std::ostream& os, Grid const& grid) -> std::ostream&;

  void push_back(std::string const& row) { rows_.push_back(row); }

  void spin_cycle()
  {
    for (unsigned i = 0; i < 4; ++i) {
      roll_north();
      rotate_right();
    }
  }

  [[nodiscard]] auto load() const -> UInt
  {
    UInt load{0};
    for (std::size_t row{0}; row != rows_.size(); ++row) {
      auto const count = std::ranges::count_if(rows_[row].begin(), rows_[row].end(),
                                               [](char c) { return c == 'O'; });
      load += count * (rows_.size() - row);
    }
    return load;
  }

  [[nodiscard]] auto hash() const -> std::size_t
  {
    return std::hash<std::string>{}(std::accumulate(rows_.begin(), rows_.end(), std::string(),
                                                    [](auto const& a, auto const& b) {
                                                      return a + b;
                                                    }));
  }

  [[nodiscard]] auto operator==(Grid const& rhs) const -> bool { return rows_ == rhs.rows_; }

private:
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

  void rotate_right()
  {
    std::vector<std::string> new_rows;
    new_rows.resize(rows_.front().size());
    for (auto row{rows_.rbegin()}; row != rows_.rend(); ++row) {
      for (std::size_t i{0}; i < row->size(); ++i) {
        new_rows[i].push_back(row->at(i));
      }
    }
    std::swap(new_rows, rows_);
  }

  std::vector<std::string> rows_;
};

template<>
struct std::hash<Grid>
{
  [[nodiscard]] auto operator()(Grid const& grid) const noexcept -> std::size_t
  {
    return grid.hash();
  }
};

auto operator<<(std::ostream& os, Grid const& grid) -> std::ostream&
{
  for (auto const& row : grid.rows_) { os << row << '\n'; }
  return os;
}

auto main() -> int try {
  std::string line;
  std::unordered_map<Grid, UInt> cycle_length;

  Grid grid;
  while (std::getline(std::cin, line)) {
    grid.push_back(line);
  }

  std::cout << "Read in grid:\n" << grid;
  constexpr UInt cycle_count{1'000'000'000};
  for (UInt cycle{0}; cycle < cycle_count - 1; ++cycle) {
    auto [it, success] = cycle_length.insert({grid, cycle});
    if (!success) {
      // We have a loop - so just skip ahead a whole number of loops.
      // If I was being clever and my brain was working today I would find the exact correct grid
      // in the unordered_map `cycle_length` because we know what it will be, and break here.
      // But I'm not and so we'll just clear the cycle_length grid and work out where in the loop
      // we are by hand.  Its not noticeable even on debug.
      std::cout << "Found a loop, cycle " << it->second << " is the same as cycle " << cycle <<
        '\n';
      UInt const loop_length = cycle - it->second;
      UInt const cycles_left = cycle_count - cycle;
      UInt const loops_left = cycles_left / loop_length;
      cycle += loops_left * loop_length;
      // Clear the unordered map because we're being lazy.
      cycle_length.clear();
    }
    grid.spin_cycle();
  }
  grid.spin_cycle();
  std::cout << "Rolled grid:\n" << grid;
  std::cout << "Load: " << grid.load() << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
