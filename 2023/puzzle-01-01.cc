#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

using UInt = std::uint64_t;

constexpr std::array nums = {
  std::make_pair("one", 1), std::make_pair("two", 2), std::make_pair("three", 3),
  std::make_pair("four", 4),
  std::make_pair("five", 5), std::make_pair("six", 6),
  std::make_pair("seven", 7),
  std::make_pair("eight", 8), std::make_pair("nine", 9)};

auto find_first_digit(std::string const& line) -> int
{
  auto pos = line.find_first_of("0123456789");
  auto value = line[pos] - '0';
  for (auto [s, v] : nums) {
    if (auto p = line.find(s); p != std::string::npos) {
      if (p < pos) {
        pos = p;
        value = v;
      }
    }
  }
  return value;
}

auto find_last_digit(std::string const& line) -> int
{
  auto pos = line.find_last_of("0123456789");
  auto value = line[pos] - '0';
  for (auto [s, v] : nums) {
    if (auto p = line.rfind(s); p != std::string::npos) {
      if (p > pos) {
        pos = p;
        value = v;
      }
    }
  }
  return value;
}

auto main() -> int
{
  UInt sum1{0};
  UInt sum2{0};

  std::string line;
  while (std::getline(std::cin, line)) {
    auto first = line.find_first_of("0123456789");
    auto last = line.find_last_of("0123456789");
    assert(first != std::string::npos);
    assert(last != std::string::npos);
    sum1 += (line[first] - '0') * 10;
    sum1 += (line[last] - '0');
    sum2 += find_first_digit(line) * 10;
    sum2 += find_last_digit(line);
  }

  std::cout << "Sum 1: " << sum1 << "\n";
  std::cout << "Sum 2: " << sum2 << "\n";
  return EXIT_SUCCESS;
}
