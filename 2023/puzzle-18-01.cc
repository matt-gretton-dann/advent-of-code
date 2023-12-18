#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

using Int = std::int64_t;
using UInt = std::uint64_t;

enum class Dir { horiz, vert };

struct Location
{
  Location(Int const x, Int const y) noexcept
    : x_(x), y_(y)
  {
  }

  [[nodiscard]] auto operator==(Location const& rhs) const noexcept -> bool
  {
    return x_ == rhs.x_ && y_ == rhs.y_;
  }

  [[nodiscard]] auto x() const noexcept -> Int { return x_; }
  [[nodiscard]] auto y() const noexcept -> Int { return y_; }

private:
  Int x_;
  Int y_;
};

struct Data
{
  Data(Dir const dir, UInt const colour) noexcept
    : dir_(dir), colour_(colour)
  {
  }

  void dir(Dir const dir) noexcept { dir_ = dir; }
  [[nodiscard]] auto dir() const noexcept -> Dir { return dir_; }
  [[nodiscard]] auto colour() const noexcept -> UInt { return colour_; }

private:
  Dir dir_;
  UInt colour_;
};

template<>
struct std::hash<Location>
{
  [[nodiscard]] auto operator()(Location const& loc) const noexcept -> std::size_t
  {
    return std::hash<Int>{}((loc.x() << 16) + loc.y());
  }
};

struct Grid
{
  void add_instruction(std::string const& line)
  {
    auto const dir = line[0];
    char* pos = nullptr;
    UInt const amt = std::strtoull(line.data() + 2, &pos, 10);
    pos += 3;
    UInt const colour = std::strtoull(pos, &pos, 16);

    auto const vert = (dir == 'U' || dir == 'D') ? Dir::vert : Dir::horiz;
    Data const data(vert, colour);

    if (auto const it = edges_.find(current_loc_); it != edges_.end()) {
      it->second.dir((entered_from_north || dir == 'U') ? Dir::vert : Dir::horiz);
    }

    Int const dx = dir == 'R' ? 1 : (dir == 'L' ? -1 : 0);
    Int const dy = dir == 'D' ? 1 : (dir == 'U' ? -1 : 0);
    for (unsigned i = 0; i < amt; ++i) {
      current_loc_ = Location(current_loc_.x() + dx, current_loc_.y() + dy);
      auto [it, success] = edges_.insert({current_loc_, data});
      assert(success);
    }

    min_y_ = std::min(min_y_, current_loc_.y());
    max_y_ = std::max(max_y_, current_loc_.y());
    min_x_ = std::min(min_x_, current_loc_.x());
    max_x_ = std::max(max_x_, current_loc_.x());

    entered_from_north = dir == 'D';
  }

  [[nodiscard]] auto count_interior() noexcept -> UInt
  {
    if (auto const it = edges_.find(current_loc_); it != edges_.end()) {
      it->second.dir((entered_from_north) ? Dir::vert : Dir::horiz);
    }

    UInt count{0};
    for (Int y{min_y_}; y < max_y_ + 1; ++y) {
      bool inside{false};
      for (Int x{min_x_}; x < max_x_ + 1; ++x) {
        auto it = edges_.find(Location(x, y));
        if (it == edges_.end()) {
          count += inside ? 1 : 0;
          std::cout << (inside ? 'X' : '.');
        }
        else {
          ++count;
          if (it->second.dir() == Dir::vert) { inside = !inside; }
          std::cout << (it->second.dir() == Dir::vert ? '|' : '-');
        }
      }
      std::cout << '\n';
    }

    return count;
  }

private:
  Location current_loc_{0, 0};
  bool entered_from_north{false};
  Int min_y_{0};
  Int max_y_{0};
  Int min_x_{0};
  Int max_x_{0};
  std::unordered_map<Location, Data> edges_;
};

auto main() -> int try {
  std::string line;
  Grid grid;

  while (std::getline(std::cin, line)) {
    grid.add_instruction(line);
  }

  UInt const count{grid.count_interior()};
  std::cout << "Total: " << count << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
