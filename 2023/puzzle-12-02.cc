#include <cstdint>
#include <cstdlib>
#include <iostream>
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

[[nodiscard]] auto validate_subpattern(std::string_view pattern, std::string_view start,
                                       UInt num_hashs, UInt gaps) -> bool
{
  if (pattern.size() < start.size()) {
    return false;
  }
  UInt hash_count{0};

  auto pit{pattern.begin()};
  for (auto sit{start.begin()}; sit != start.end(); ++pit, ++sit) {
    if (*sit == '#') { ++hash_count; }
    if (*pit != '?' && *pit != *sit) {
      return false;
    }
  }

  for (; pit != pattern.end(); ++pit) {
    if (*pit == '#') { ++hash_count; }
  }

  if (num_hashs < hash_count) {
    return false;
  }
  if (start.size() + (num_hashs - hash_count) + gaps > pattern.size()) {
    return false;
  }

  return true;
}

void generate_pattern(std::string start, UIntVec::const_iterator it, UIntVec::const_iterator end,
                      std::string_view pattern, UInt hash_count, UInt& count)
{
  for (std::size_t p{0}; p + start.size() < pattern.size() && p < *it; ++p) {
    if (pattern[start.size() + p] == '.') { return; }
  }
  start += std::string(*it++, '#');

  if (it == end) {
    if (start.size() <= pattern.size()) {
      start.resize(pattern.size(), '.');
      if (validate_subpattern(pattern, start, hash_count, end - it - 1)) {
        ++count;
        if (count % 100'000 == 0) {
          std::cout << start << '\n';
        }
      }
    }
    return;
  }

  start += '.';
  while (validate_subpattern(pattern, start, hash_count, end - it - 1)) {
    generate_pattern(start, it, end, pattern, hash_count, count);
    start += '.';
  }
}

auto valid_sequences(std::string_view pattern, UIntVec const& nums) -> UInt
{
  UInt count{0};
  UInt hash_count{0};
  for (auto num : nums) { hash_count += num; }
  std::string start;
  while (validate_subpattern(pattern, start, hash_count, nums.size() - 1)) {
    std::cout << start << '\n';
    generate_pattern(start, nums.begin(), nums.end(), pattern, hash_count, count);
    start += '.';
  }

  return count;
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

    line_count += valid_sequences(unfolded_pattern, unfolded_nums);
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
