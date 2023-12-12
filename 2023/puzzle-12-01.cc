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

[[nodiscard]] auto validate_pattern(std::string_view pattern, std::string_view line) -> bool
{
  if (pattern.size() != line.size()) { return false; }

  for (auto pit{pattern.begin()}, lit{line.begin()}; pit != pattern.end(); ++pit, ++lit) {
    if (*pit != '?' && *pit != *lit) { return false; }
  }

  return true;
}

void generate_pattern(std::string start, UIntVec::const_iterator it, UIntVec::const_iterator end,
                      std::string_view pattern, UInt& count)
{
  start += std::string(*it++, '#');

  if (it == end) {
    if (start.size() <= pattern.size()) {
      start.resize(pattern.size(), '.');
      count += validate_pattern(pattern, start) ? 1 : 0;
    }
    return;
  }

  while (start.size() < pattern.size()) {
    start += '.';
    generate_pattern(start, it, end, pattern, count);
  }
}

auto valid_sequences(std::string_view pattern, UIntVec const& nums) -> UInt
{
  UInt count{0};
  std::string start;
  while (start.size() < pattern.size()) {
    generate_pattern(start, nums.begin(), nums.end(), pattern, count);
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
    line_count += valid_sequences(pattern, nums);
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
