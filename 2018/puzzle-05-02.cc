//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <utility>

using UInt = std::uint64_t;

using namespace std::string_literals;

auto amend_polymer(std::string polymer, char remove) -> std::string
{
  auto it{std::remove_if(polymer.begin(), polymer.end(),
                         [remove](auto c) { return c == remove || (c == std::toupper(remove)); })};
  return {polymer.begin(), it};
}

auto react_polymer(std::string polymer) -> UInt
{
  auto it{polymer.begin()};
  while ((it + 1) != polymer.end()) {
    if (*it != *(it + 1) && (*it == std::tolower(*(it + 1)) || *it == std::toupper(*(it + 1)))) {
      it = polymer.erase(it, it + 2);
      if (it != polymer.begin()) {
        --it;
      }
    }
    else {
      ++it;
    }
  }
  return polymer.size();
}

auto main() -> int
{
  std::string polymer;

  if (!std::getline(std::cin, polymer)) {
    std::cout << "Unable to read line.\n";
    return EXIT_FAILURE;
  }

  UInt min_length{polymer.size() + 1};
  for (char c{'a'}; c < 'z' + 1; ++c) {
    std::string const poly2{amend_polymer(polymer, c)};
    min_length = std::min(min_length, react_polymer(poly2));
  }
  std::cout << "Minimum length: " << min_length << "\n";
  return EXIT_SUCCESS;
}
