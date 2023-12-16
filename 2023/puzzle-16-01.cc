#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

using Location = std::pair<Int, Int>;
using Direction = std::pair<Int, Int>;
constexpr Direction north = std::make_pair(0, -1);
constexpr Direction east = std::make_pair(1, 0);
constexpr Direction south = std::make_pair(0, 1);
constexpr Direction west = std::make_pair(-1, 0);

auto operator+(Location const& loc, Direction const& dir) -> Location
{
  return std::make_pair(loc.first + dir.first, loc.second + dir.second);
}

struct Beam
{
  Beam(Location loc, Direction dir)
    : loc_(std::move(loc)), dir_(std::move(dir))
  {
  }

  [[nodiscard]] auto hash() const noexcept -> std::size_t
  {
    return std::hash<Int>{}(
      ((loc_.first * 1000 + loc_.second) * 4 + (dir_.first + 1)) * 4 + (dir_.second + 1));
  }

  [[nodiscard]] auto operator==(Beam const& rhs) const noexcept -> bool
  {
    return loc_ == rhs.loc_ && dir_ == rhs.dir_;
  }

  Location loc_;
  Direction dir_;
};

template<>
struct std::hash<Beam>
{
  [[nodiscard]] auto operator()(Beam const& beam) const noexcept -> std::size_t
  {
    return beam.hash();
  }
};

template<>
struct std::hash<Location>
{
  [[nodiscard]] auto operator()(Location const& loc) const noexcept -> std::size_t
  {
    return std::hash<Int>{}(loc.first * 1000 + loc.second);
  }
};

struct Grid
{
  void push_line(std::string const& line) { grid_.push_back(line); }

  [[nodiscard]] auto energised() const -> UInt
  {
    // The energised set includes directions - so we need to build a set that just includes locations.
    std::unordered_set<Location> locs;
    std::vector<std::string> energy_grid(grid_.size(), std::string(grid_[0].size(), '.'));
    for (auto const [loc, dir] : energised_) {
      locs.insert(loc);
      energy_grid[loc.second][loc.first] = '#';
    }
    for (auto const& row : energy_grid) { std::cout << row << '\n'; }
    return locs.size();
  }

  [[nodiscard]] auto loc_valid(Location const& loc) const noexcept -> bool
  {
    if (loc.first < 0) { return false; }
    if (loc.second < 0) { return false; }
    if (loc.second >= grid_.size()) { return false; }
    if (loc.first >= grid_[loc.second].size()) { return false; }
    return true;
  }

  void energise()
  {
    std::list<Beam> beams;
    beams.emplace_back(std::make_pair(0, 0), east);

    while (!beams.empty()) {
      auto beam{beams.front()};
      beams.pop_front();
      if (!loc_valid(beam.loc_)) { continue; }
      // Insert into hash of energised locations.  If that fails we've done this before and don't
      // need to do it again.
      if (auto [it, success] = energised_.insert(beam); !success) { continue; }

      switch (grid_[beam.loc_.second][beam.loc_.first]) {
      case '.':
        break;
      case '\\':
        if (beam.dir_ == north) { beam.dir_ = west; }
        else if (beam.dir_ == east) { beam.dir_ = south; }
        else if (beam.dir_ == south) { beam.dir_ = east; }
        else if (beam.dir_ == west) { beam.dir_ = north; }
        else { std::abort(); }
        break;
      case '/':
        if (beam.dir_ == north) { beam.dir_ = east; }
        else if (beam.dir_ == east) { beam.dir_ = north; }
        else if (beam.dir_ == south) { beam.dir_ = west; }
        else if (beam.dir_ == west) { beam.dir_ = south; }
        else { std::abort(); }
        break;
      case '|':
        if (beam.dir_ == east || beam.dir_ == west) {
          Beam beam2{beam};
          beam2.dir_ = north;
          beam2.loc_ = beam2.loc_ + beam2.dir_;
          beams.push_back(beam2);
          beam.dir_ = south;
        }
        break;
      case '-':
        if (beam.dir_ == north || beam.dir_ == south) {
          Beam beam2{beam};
          beam2.dir_ = east;
          beam2.loc_ = beam2.loc_ + beam2.dir_;
          beams.push_back(beam2);
          beam.dir_ = west;
        }
        break;
      default:
        std::abort();
      }

      beam.loc_ = beam.loc_ + beam.dir_;
      beams.push_back(beam);
    }
  }

private:
  std::vector<std::string> grid_;
  std::unordered_set<Beam> energised_;
};

auto main() -> int try {
  std::string line;
  Grid grid;

  while (std::getline(std::cin, line)) {
    grid.push_line(line);
  }

  grid.energise();
  UInt count = grid.energised();
  std::cout << "Total: " << count << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
