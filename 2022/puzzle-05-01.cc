//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <cassert>

using UInt = std::uint64_t;

auto main() -> int
{
  std::string line;
  std::vector<std::string> stacks(11, "");
  while (std::getline(std::cin, line) && !line.empty()) {
    UInt col{1};
    UInt c4{0};
    for (auto c : line) {
      ++c4;
      if (c4 == 4) {
        ++col;
        c4 = 0;
      }
      else if (std::isupper(c) == 1) {
        stacks[col].push_back(c);
      }
    }
  }

  std::cout << "Initial stacks:\n";
  for (auto const& s : stacks) {
    std::cout << s << '\n';
  }

  while (std::getline(std::cin, line)) {
    static std::regex const re{R"(move (\d+) from (\d+) to (\d+))"};
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << "\n";
      return EXIT_FAILURE;
    }
    UInt const num{std::stoull(m.str(1))};
    UInt const from{std::stoull(m.str(2))};
    UInt const to{std::stoull(m.str(3))};
    assert(stacks[from].size() >= num);
    auto s = stacks[from].substr(0, num);
    std::reverse(s.begin(), s.end());
    stacks[from] = stacks[from].substr(num);
    stacks[to] = s + stacks[to];
  }

  std::cout << "Final stacks:\n";
  for (auto const& s : stacks) {
    std::cout << s << '\n';
  }

  std::cout << "Result: ";
  for (auto s : stacks) {
    if (!s.empty()) {
      std::cout << s.front();
    }
  }
  std::cout << '\n';

  return 0;
}
