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
using StringVec = std::vector<std::string>;
using UIntVec = std::vector<UInt>;
using Pos = std::pair<UInt, UInt>;
using PosVec = std::vector<Pos>;

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

auto valid_sequences(std::string_view pattern, UIntVec::const_iterator begin,
                     UIntVec::const_iterator end) -> UInt;

auto generate_pattern(std::string pattern, UIntVec::const_iterator begin,
                      UIntVec::const_iterator end) -> UInt
{
  while (begin != end) {
    if (pattern.empty()) {
      // We still have ###s to add but nowhere to put them - this isn't a match.
      return 0;
    }

    if (pattern[0] == '?') {
      // We don't know what the first character is so let's try both options recursively.
      auto new_pattern{pattern};
      new_pattern[0] = '.';
      auto count = valid_sequences(new_pattern, begin, end);
      new_pattern[0] = '#';
      count += valid_sequences(new_pattern, begin, end);
      return count;
    }

    if (pattern[0] == '.') {
      // If the pattern begins with a '.' then we just skip over it.
      pattern = pattern.substr(1);
      continue;
    }

    /* We must have a '#' now. */
    assert(pattern[0] == '#');

    if (pattern.size() < *begin) {
      // Not enough space.
      return 0;
    }

    for (std::size_t count = 0; count < *begin; ++count) {
      // Check for dots.
      if (pattern[count] == '.') { return 0; }
    }

    // Move along the pattern.
    pattern = pattern.substr(*begin++);

    if (begin != end) {
      if (pattern.empty()) { return 0; }
      if (pattern[0] == '#') { return 0; }
      pattern = pattern.substr(1);
    }
  }

  return pattern.find('#') == std::string_view::npos;
}

auto valid_sequences(std::string_view pattern, UIntVec::const_iterator begin,
                     UIntVec::const_iterator end) -> UInt
{
  std::string hash = std::string(pattern);
  for (auto it{begin}; it != end; ++it) {
    hash += ',';
    hash += std::to_string(*it);
  }

  auto it = counts.find(hash);
  if (it == counts.end()) {
    bool success;
    std::tie(it, success) = counts.insert(
      {hash, generate_pattern(std::string(pattern), begin, end)});
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

    line_count += valid_sequences(unfolded_pattern, unfolded_nums.begin(), unfolded_nums.end());
    std::cout << line << ": " << line_count << '\n';
    count += line_count;
  }

  std::cout << "Valid sequences: " << count << '\n';

  return EXIT_SUCCESS;;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
