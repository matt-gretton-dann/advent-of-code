//
// Created by Matthew Gretton-Dann on 01/12/2021.
//

#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using Int = std::int64_t;

auto main() -> int
{
  Int value{0};
  std::string line;
  std::set<Int> freqs;
  std::vector<Int> input;
  freqs.insert(0);

  while (std::getline(std::cin, line)) {
    input.emplace_back(std::stoll(line));
  }

  while (true) {
    for (auto incr : input) {
      value += incr;
      auto [it, success] = freqs.insert(value);
      if (!success) {
        std::cout << "Double frequency seen at: " << value << '\n';
        return 0;
      }
    }
  }
}
