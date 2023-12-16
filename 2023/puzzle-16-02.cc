#include <algorithm>
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

  [[nodiscard]] auto max_energised() -> UInt
  {
    UInt max{0};
    for (unsigned x{0}; x < grid_[0].size(); ++x) {
      energised_.clear();
      energise(Beam(std::make_pair(x, 0), south));
      max = std::max(max, energised());
    }

    for (unsigned y{0}; y < grid_.size(); ++y) {
      energised_.clear();
      energise(Beam(std::make_pair(0, y), east));
      max = std::max(max, energised());

      energised_.clear();
      energise(Beam(std::make_pair(grid_[y].size() - 1, y), west));
      max = std::max(max, energised());
    }

    auto max_y{grid_.size() - 1};
    for (unsigned x{0}; x < grid_[max_y].size(); ++x) {
      energised_.clear();
      energise(Beam(std::make_pair(x, max_y), north));
      max = std::max(max, energised());
    }

    return max;
  }

private:
  [[nodiscard]] auto energised() const -> UInt
  {
    // The energised set includes directions - so we need to build a set that just includes locations.
    std::unordered_set<Location> locs;
    for (auto const [loc, dir] : energised_) {
      locs.insert(loc);
    }
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

  void energise(Beam const& start)
  {
    std::list<Beam> beams;
    beams.push_back(start);

    while (!beams.empty()) {
      auto beam{beams.front()};
      beams.pop_front();
      while (loc_valid(beam.loc_) && energised_.insert(beam).second) {
        switch (grid_[beam.loc_.second][beam.loc_.first]) {
        case '.':
          break;
        case '\\':
          std::swap(beam.dir_.first, beam.dir_.second);
          break;
        case '/':
          std::swap(beam.dir_.first, beam.dir_.second);
          beam.dir_.first = -beam.dir_.first;
          beam.dir_.second = -beam.dir_.second;
          break;
        case '|':
          if (beam.dir_.second == 0) {
            Beam beam2{beam};
            beam2.dir_ = north;
            beam2.loc_ = beam2.loc_ + beam2.dir_;
            beams.push_back(beam2);
            beam.dir_ = south;
          }
          break;
        case '-':
          if (beam.dir_.first == 0) {
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
      }
    }
  }

  std::vector<std::string> grid_;
  std::unordered_set<Beam> energised_;
};

auto main() -> int try {
  std::string line;
  Grid grid;

  while (std::getline(std::cin, line)) {
    grid.push_line(line);
  }

  UInt max = grid.max_energised();
  std::cout << "Total: " << max << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
