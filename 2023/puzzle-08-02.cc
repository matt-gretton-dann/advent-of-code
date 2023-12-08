#include <array>
#include <cassert>
#include <compare>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using UInt = std::uint64_t;
using Location = std::array<char, 3>;
using LocationPair = std::pair<Location, Location>;

auto operator<<(std::ostream& os, Location const& cards) -> std::ostream&
{
  for (auto const c : cards) { os << c; }
  return os;
}

[[nodiscard]] auto operator<=>(Location const& lhs, Location const& rhs) -> std::strong_ordering
{
  for (auto lhs_it{lhs.begin()}, rhs_it{rhs.begin()}; lhs_it != lhs.end(); ++lhs_it, ++rhs_it) {
    if (*lhs_it != *rhs_it) {
      return *lhs_it <=> *rhs_it;
    }
  }
  return std::strong_ordering::equal;
}

struct LocationMap
{
  void emplace(std::string_view const line)
  {
    auto from = Location{line[0], line[1], line[2]};
    auto left = Location{line[7], line[8], line[9]};
    auto right = Location{line[12], line[13], line[14]};
    auto [it, success] = locs_.insert({from, {left, right}});
    assert(success);
  }

  using const_iterator = std::map<Location, LocationPair>::const_iterator;
  std::map<Location, LocationPair> locs_;
};

auto next_location(LocationMap const& map, LocationMap::const_iterator const loc,
                   char const d) -> LocationMap::const_iterator
{
  if (d == 'L') {
    return map.locs_.find(loc->second.first);
  }
   if (d == 'R') {
    return map.locs_.find(loc->second.second);
  }
    std::abort();
}

auto main() -> int try {
  std::string line;
  std::string directions;
  LocationMap map;
  std::vector<LocationMap::const_iterator> locations;

  std::getline(std::cin, directions); /// Directions line
  std::getline(std::cin, line);       /// Blank line

  while (std::getline(std::cin, line)) {
    map.emplace(line);
    if (line[2] == 'A') {
      locations.emplace_back(map.locs_.find({line[0], line[1], line[2]}));
    }
  }

  UInt total_steps = 0;
  UInt total_cycle = 1;

  std::cout << "Start: " << total_steps << " steps and " << total_cycle << " cycle.\n";

  // Big assumption time.
  // Let's assume there's a unique Z for each A and we don't see multiple on the cycle.
  for (auto location : locations) {
    UInt steps = 0;
    auto direction = directions.begin();

std::cout << "From: " << location->first;
    // How many steps to get to the first '..Z'
    while (location->first[2] != 'Z') {
      location = next_location(map, location, *direction++);
      if (direction == directions.end()) { direction = directions.begin(); }
      ++steps;
    }

    // Step forward one and start counting how many steps until we return to the 'Z'.
    auto z_loc = location;
    std::cout << " to: " << z_loc->first << " in " << steps << " steps, cycles in: ";
    location = next_location(map, location, *direction++);
    if (direction == directions.end()) { direction = directions.begin(); }
    UInt cycle = 1;

    while (location->first[2] != 'Z') {
      location = next_location(map, location, *direction++);
      if (direction == directions.end()) { direction = directions.begin(); }
      ++cycle;
    }

    // Check we've returned to the same 'Z'.
    if (location != z_loc) { std::abort(); }
    std::cout << cycle << '\n';

    while (total_steps != steps) {
      if (total_steps < steps) { total_steps += total_cycle; }
      else {
        assert(steps < total_steps);
        {
          steps += cycle;
        }
      }
    }
    total_cycle = std::lcm(total_cycle, cycle);
  std::cout << "Meging: " << total_steps << " steps and " << total_cycle << " cycle.\n";

  }

  std::cout << "Steps: " << total_steps;

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
