//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using Int = std::int64_t;

auto str_dist(std::string const& s1, std::string const& s2) noexcept -> Int
{
  Int result{0};
  for (std::size_t i{0}; i != s1.size(); ++i) {
    if (s1[i] != s2[i]) {
      ++result;
    }
  }
  return result;
}

auto main() -> int
{
  std::string line;
  std::vector<std::string> lines;
  while (std::getline(std::cin, line)) {
    lines.push_back(line);
  }

  Int closest{std::numeric_limits<Int>::max()};
  std::string s1;
  std::string s2;
  for (auto it{lines.begin()}; it != lines.end(); ++it) {
    for (auto it2{it + 1}; it2 != lines.end(); ++it2) {
      auto distance{str_dist(*it, *it2)};
      if (distance < closest) {
        closest = distance;
        s1 = *it;
        s2 = *it2;
      }
    }
  }
  std::string result;
  result.reserve(s1.size());
  for (std::size_t i{0}; i != s1.size(); ++i) {
    if (s1[i] == s2[i]) {
      result += s1[i];
    }
  }

  std::cout << "Result: " << result << '\n';
  return EXIT_SUCCESS;
}
