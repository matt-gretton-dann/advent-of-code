#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using UInt = std::uint64_t;
using NumVec = std::vector<UInt>;
using Range = std::pair<UInt, UInt>;
using RangeVec = std::vector<Range>;

auto operator<<(std::ostream& os, RangeVec const& ranges) -> std::ostream&
{
  for (auto [start, length] : ranges) {
    os << " [" << start << '+' << length << ']';
  }
  return os;
}

template<typename It>
void read_nums(It it, std::string_view const nums)
{
  char const* pos{nums.data()};
  while (pos - nums.data() < nums.size()) {
    char* next_pos{nullptr};
    *it++ = std::strtoull(pos, &next_pos, 10);
    pos = next_pos;
  }
}

template<typename It>
void read_ranges(It it, std::string_view const nums)
{
  char const* pos{nums.data()};
  while (pos - nums.data() < nums.size()) {
    char* next_pos{nullptr};
    auto start = std::strtoull(pos, &next_pos, 10);
    pos = next_pos;
    auto length = std::strtoull(pos, &next_pos, 10);
    pos = next_pos;
    *it++ = std::make_pair(start, length);
  }
}

struct MapEntry
{
  explicit MapEntry(NumVec const& vec)
    : dest_(vec[0]), source_(vec[1]), length_(vec[2])
  {
    assert(vec.size() == 3);
  }

  UInt dest_;
  UInt source_;
  UInt length_;
};

struct ThingyMap
{
  void add(std::string_view const line)
  {
    NumVec vec;
    read_nums(std::back_inserter(vec), line);
    assert(vec.size() == 3);
    entries_.emplace_back(vec);
  }

  template<typename It>
  void transform(It it, UInt const range_start, UInt const range_length) const
  {
    /* Look for a start range that starts inside a map entry. */
    for (auto const [dest, source, length] : entries_) {
      if (range_start < source) { continue; }
      if (range_start >= source + length) { continue; }
      auto const offset = range_start - source;
      auto const new_length = std::min(range_length, length - offset);
      assert(new_length != 0);
      *it++ = std::make_pair(dest + offset, new_length);
      if (new_length != range_length) {
        transform(it, range_start + new_length, range_length - new_length);
      }
      return;
    }

    /* Now look for an end that ends inside a map entry. */
    for (auto const [dest, source, length] : entries_) {
      auto const range_end = range_start + range_length;
      if (range_end <= source) { continue; }
      if (range_end > source + length) { continue; }
      assert(source > range_start);
      auto const new_length = source - range_start;
      assert(new_length != 0);
      *it++ = std::make_pair(range_start, new_length);
      if (range_length != new_length) {
        transform(it, range_start + new_length, range_length - new_length);
      }
      return;
    }

    *it++ = std::make_pair(range_start, range_length);
  }

  std::vector<MapEntry> entries_;
};

auto main() -> int try {
  std::string line;
  RangeVec seeds{};
  std::getline(std::cin, line);

  // Read the seeds.
  assert(line.substr(0, 6) == "seeds:");
  read_ranges(std::back_inserter(seeds), line.substr(6));
  std::cout << "Seeds:" << seeds << '\n';

  // Skip whitespace
  std::getline(std::cin, line);

  while (std::getline(std::cin, line)) {
    std::cout << line << ":";
    ThingyMap map;
    while (std::getline(std::cin, line)) {
      if (line.empty()) { break; }
      map.add(line);
    }
    RangeVec transformed;

    for (auto [start, length] : seeds) {
      map.transform(std::back_inserter(transformed), start, length);
    }
    std::cout << transformed << '\n';
    seeds = transformed;
  }

  std::cout << "Minimum: " << std::ranges::min_element(seeds.begin(), seeds.end(),
                                                       [](Range const& left, Range const& right) {
                                                         return left.first < right.first;
                                                       })->first << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
