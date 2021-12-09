//
// Created by Matthew Gretton-Dann on 04/12/2021.
//

#include <array>
#include <cassert>
#include <iostream>
#include <numeric>
#include <string>

struct Display
{
  static constexpr unsigned width{50};
  static constexpr unsigned height{6};

  [[nodiscard]] auto lit_count() const noexcept -> unsigned
  {
    return std::accumulate(grid_.begin(), grid_.end(), unsigned{0},
                           [](unsigned a, auto i) { return a + (i ? 1 : 0); });
  }

  auto rect(unsigned w, unsigned h) -> Display
  {
    assert(w < width);
    assert(h < height);
    Display n{*this};
    for (unsigned i{0}; i < h; ++i) {
      for (unsigned j{0}; j < w; ++j) {
        n.grid_.at(i * width + j) = true;
      }
    }
    return n;
  }

  auto rotate_column(unsigned col, unsigned amt) -> Display
  {
    assert(col < width);
    amt %= height;
    Display n(*this);
    for (unsigned i{0}; i < height; ++i) {
      auto ni = (i + amt) % height;
      n.grid_.at(ni * width + col) = grid_.at(i * width + col);
    }
    return n;
  }

  auto rotate_row(unsigned row, unsigned amt) -> Display
  {
    assert(row < height);
    amt %= width;
    Display n(*this);
    for (unsigned i{0}; i < width; ++i) {
      auto ni = (i + amt) % width;
      n.grid_.at(ni + row * width) = grid_.at(i + row * width);
    }
    return n;
  }

  void print(std::ostream& os) const
  {
    for (unsigned i = 0; i < width * height; ++i) {
      if (i % 5 == 0) {
        std::cout << " | ";
      }
      os << (grid_[i] ? '*' : ' ');
      if (i % width == (width - 1)) {
        std::cout << '\n';
      }
    }
  }

private:
  std::array<bool, static_cast<std::size_t>(width* height)> grid_{false};
};

auto begins_with(std::string const& s, std::string const& b) { return s.substr(0, b.size()) == b; }

auto main() -> int
{
  const std::string rect_begin{"rect "};
  const std::string rotate_col_begin{"rotate column x="};
  const std::string rotate_row_begin{"rotate row y="};
  Display display{};
  std::string line;
  while (std::getline(std::cin, line)) {
    if (begins_with(line, rect_begin)) {
      auto idx{rect_begin.size()};
      std::size_t end{0};
      auto width{std::stoul(line.substr(idx), &end)};
      idx += end + 1;
      auto height{std::stoul(line.substr(idx))};
      display = display.rect(width, height);
    }
    else if (begins_with(line, rotate_col_begin)) {
      auto idx{rotate_col_begin.size()};
      std::size_t end{0};
      auto col{std::stoul(line.substr(idx), &end)};
      idx += end + 4;
      auto amt{std::stoul(line.substr(idx))};
      display = display.rotate_column(col, amt);
    }
    else if (begins_with(line, rotate_row_begin)) {
      auto idx{rotate_row_begin.size()};
      std::size_t end{0};
      auto row{std::stoul(line.substr(idx), &end)};
      idx += end + 4;
      auto amt{std::stoul(line.substr(idx))};
      display = display.rotate_row(row, amt);
    }
  }

  std::cout << "Number of lit fields: " << display.lit_count() << '\n';
  display.print(std::cout);
  return 0;
}
