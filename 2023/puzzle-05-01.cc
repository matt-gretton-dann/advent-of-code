#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

using UInt = std::uint64_t;
using NumVec = std::vector<UInt>;

auto operator<<(std::ostream& os, NumVec const& vec) -> std::ostream&
{
  for (auto const num : vec) { os << ' ' << num; }
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
  void add(std::string_view line)
  {
    NumVec vec;
    read_nums(std::back_inserter(vec), line);
    assert(vec.size() == 3);
    entries_.emplace_back(vec);
  }

  [[nodiscard]] auto transform(UInt const in) const -> UInt
  {
    for (auto const [dest, source, length] : entries_) {
      if (in < source) { continue; }
      if (in >= source + length) { continue; }
      return dest + (in - source);
    }
    return in;
  }

  std::vector<MapEntry> entries_;
};

auto main() -> int try {
  std::string line;
  NumVec seeds{};
  std::getline(std::cin, line);

  // Read the seeds.
  assert(line.substr(0, 6) == "seeds:");
  read_nums(std::back_inserter(seeds), line.substr(6));
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
    NumVec transformed;

    std::transform(seeds.begin(), seeds.end(), std::back_inserter(transformed),
                   [&map](UInt in) { return map.transform(in); });
    std::cout << transformed << '\n';
    seeds = transformed;
  }

  std::cout << "Minimum: " << *std::ranges::min_element(seeds.begin(), seeds.end()) << '\n';;

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
