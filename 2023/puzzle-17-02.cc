#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

enum class Dir { horiz, vert };

constexpr UInt dir_min{4};
constexpr UInt dir_max{11};

struct Location
{
  Location(UInt const x, UInt const y, Dir const dir) noexcept
    : x_(x), y_(y), dir_(dir)
  {
  }

  [[nodiscard]] auto x() const noexcept -> UInt { return x_; }
  [[nodiscard]] auto y() const noexcept -> UInt { return y_; }
  [[nodiscard]] auto dir() const noexcept -> Dir { return dir_; }

private:
  UInt x_;
  UInt y_;
  Dir dir_;
};

auto operator<<(std::ostream& os, Location const& loc) -> std::ostream&
{
  return os << '(' << loc.x() << ", " << loc.y() << ", " << (loc.dir() == Dir::horiz ? '-' : '|') <<
         ')';
}

struct Grid
{
  void push_line(std::string const& line) { grid_.push_back(line); }

  [[nodiscard]] auto find_route() -> UInt
  {
    reset_state();
    add_location(Location{0, 0, Dir::horiz}, 0);
    add_location(Location{0, 0, Dir::vert}, 0);

    while (!work_list_.empty()) {
      auto it = work_list_.begin();
      std::cout << it->first << ": ";
      for (auto const& loc : it->second) {
        if (visited(loc)) { continue; }

        std::cout << loc << ", ";
        visit(loc);

        if (loc.x() == grid_[loc.y()].size() - 1 && loc.y() == grid_.size() - 1) {
          // Reached the end return the cost.
          std::cout << '\n';
          return it->first;
        }

        if (loc.dir() == Dir::horiz) { add_vert_locations(loc.x(), loc.y(), it->first); }
        else { add_horiz_locations(loc.x(), loc.y(), it->first); }
      }
      work_list_.erase(it);
      std::cout << "\n";
    }

    std::abort();
  }

private:
  void reset_state()
  {
    horiz_visited_.clear();
    vert_visited_.clear();
    work_list_.clear();

    horiz_visited_.resize(grid_.size() * grid_[0].size(), false);
    vert_visited_.resize(grid_.size() * grid_[0].size(), false);
  }

  [[nodiscard]] auto visited(Location const& loc) const noexcept -> bool
  {
    if (loc.dir() == Dir::horiz) {
      return horiz_visited_[loc.x() * grid_.size() + loc.y()];
    }

    return vert_visited_[loc.x() * grid_.size() + loc.y()];
  }

  void visit(Location const& loc) noexcept
  {
    if (loc.dir() == Dir::horiz) {
      horiz_visited_[loc.x() * grid_.size() + loc.y()] = true;
    }
    else {
      vert_visited_[loc.x() * grid_.size() + loc.y()] = true;
    }
  }

  void add_location(Location const& loc, UInt cost)
  {
    if (visited(loc)) { return; }
    auto [it, success] = work_list_.insert({cost, std::vector<Location>{}});
    it->second.push_back(loc);
  }

  void add_horiz_locations(UInt const x, UInt const y, UInt const cost)
  {
    for (UInt dcost{0}, dx{1}; dx < dir_max; ++dx) {
      if (x < dx) { break; }
      dcost += grid_cost(x - dx, y);
      if (dx >= dir_min) {
        add_location(Location(x - dx, y, Dir::horiz), cost + dcost);
      }
    }

    for (UInt dcost{0}, dx{1}; dx < dir_max; ++dx) {
      if (x + dx >= grid_[y].size()) { break; }
      dcost += grid_cost(x + dx, y);
      if (dx >= dir_min) {
        add_location(Location(x + dx, y, Dir::horiz), cost + dcost);
      }
    }
  }

  void add_vert_locations(UInt const x, UInt const y, UInt const cost)
  {
    for (UInt dcost{0}, dy{1}; dy < dir_max; ++dy) {
      if (y < dy) { break; }
      dcost += grid_cost(x, y - dy);
      if (dy >= dir_min) {
        add_location(Location(x, y - dy, Dir::vert), cost + dcost);
      }
    }

    for (UInt dcost{0}, dy{1}; dy < dir_max; ++dy) {
      if (y + dy >= grid_.size()) { break; }
      dcost += grid_cost(x, y + dy);
      if (dy >= dir_min) {
        add_location(Location(x, y + dy, Dir::vert), cost + dcost);
      }
    }
  }

  [[nodiscard]] auto grid_cost(UInt const x, UInt const y) const noexcept -> UInt
  {
    return grid_[y][x] - '0';
  }

  std::vector<std::string> grid_;
  std::vector<bool> horiz_visited_;
  std::vector<bool> vert_visited_;
  std::map<UInt, std::vector<Location>> work_list_;
};

auto main() -> int try {
  std::string line;
  Grid grid;

  while (std::getline(std::cin, line)) {
    grid.push_line(line);
  }

  UInt const cost = grid.find_route();
  std::cout << "Total: " << cost << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
