#include <array>
#include <cassert>
#include <compare>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <utility>

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

  std::map<Location, LocationPair> locs_;
};

auto main() -> int try {
  std::string line;
  std::string directions;
  LocationMap map;

  std::getline(std::cin, directions); /// Directions line
  std::getline(std::cin, line);       /// Blank line

  while (std::getline(std::cin, line)) {
    map.emplace(line);
  }

  UInt steps = 0;
  auto location = map.locs_.find({'A', 'A', 'A'});
  auto direction = directions.begin();
  while (location->first != Location{'Z', 'Z', 'Z'}) {
    if (*direction == 'L') {
      location = map.locs_.find(location->second.first);
    }
    else if (*direction == 'R') {
      location = map.locs_.find(location->second.second);
    }
    else {
      std::abort();
    }
    ++direction;
    if (direction == directions.end()) { direction = directions.begin(); }
    ++steps;
  }

  std::cout << "Steps: " << steps;

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
