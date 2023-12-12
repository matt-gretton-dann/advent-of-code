#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using UInt = std::uint64_t;
using UIntVec = std::vector<UInt>;

auto split(std::string_view line) -> std::pair<std::string_view, UIntVec>
{
  auto pos = line.find(' ');
  auto pattern = line.substr(0, pos);
  UIntVec nums;

  line = line.substr(pos);
  while (!line.empty()) {
    line = line.substr(1);
    char* n{nullptr};
    nums.emplace_back(std::strtoll(line.data(), &n, 10));
    line = line.substr(n - line.data());
  }

  return std::make_pair(pattern, nums);
}

std::unordered_map<std::string, UInt> counts;

auto valid_sequences(std::string_view const pattern, UIntVec::const_iterator begin,
                     UIntVec::const_iterator const end) -> UInt;

[[nodiscard]] auto generate_pattern(std::string_view const pattern, UIntVec::const_iterator begin,
                                    UIntVec::const_iterator const end) -> UInt
{
  std::size_t pos{0};
  auto const len{pattern.size()};
  while (begin != end) {
    if (pos == len) {
      // We still have ###s to add but nowhere to put them - this isn't a match.
      return 0;
    }

    if (pattern[pos] == '?') {
      // We don't know what the first character is so let's try both options recursively.
      std::string new_pattern{pattern.substr(pos)};
      new_pattern[0] = '.';
      auto count = valid_sequences(new_pattern, begin, end);
      new_pattern[0] = '#';
      count += valid_sequences(new_pattern, begin, end);
      return count;
    }

    if (pattern[pos] == '.') {
      // If the pattern begins with a '.' then we just skip over it.
      ++pos;
      continue;
    }

    /* We must have a '#' now. */
    assert(pattern[pos] == '#');

    if (len - pos < *begin) {
      // Not enough space.
      return 0;
    }

    for (std::size_t count = 0; count < *begin; ++count) {
      // Check for dots.
      if (pattern[pos++] == '.') { return 0; }
    }

    // Check for at least one '.' after the non-final ### sequence.
    if (++begin != end) {
      if (pos == len) { return 0; }
      if (pattern[pos] == '#') { return 0; }
      ++pos;
    }
  }

  return pattern.find('#', pos) == std::string_view::npos;
}

[[nodiscard]] auto valid_sequences(std::string_view const pattern, UIntVec::const_iterator begin,
                                   UIntVec::const_iterator const end) -> UInt
{
  // Create a string to use as a hash - because I'm too lazy to do it properly.
  // We need to store the pattern and how many sequences of #s we need to add remaining.
  // This requires us to clear counts between each top-level call to valid_sequences().
  std::string hash{pattern};
  hash += static_cast<char>(end - begin) + 'A';

  auto it = counts.find(hash);
  if (it == counts.end()) {
    bool success;
    std::tie(it, success) = counts.insert({hash, generate_pattern(pattern, begin, end)});
    assert(success);
  }

  return it->second;
}

auto main() -> int try {
  std::string line;
  UInt count{0};

  while (std::getline(std::cin, line)) {
    auto [pattern, nums] = split(line);
    UInt line_count{0};
    std::string unfolded_pattern{pattern};
    UIntVec unfolded_nums{nums.begin(), nums.end()};
    for (unsigned i = 1; i < 5; ++i) {
      unfolded_pattern += '?';
      unfolded_pattern += pattern;
      unfolded_nums.insert(unfolded_nums.begin(), nums.begin(), nums.end());
    }

    counts.clear();
    line_count += valid_sequences(unfolded_pattern, unfolded_nums.begin(), unfolded_nums.end());
    count += line_count;
  }

  std::cout << "Valid sequences: " << count << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
