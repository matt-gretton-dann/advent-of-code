//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

using UInt = std::uint64_t;

using namespace std::string_literals;

auto main() -> int
{
  std::string polymer;

  if (!std::getline(std::cin, polymer)) {
    std::cout << "Unable to read line.\n";
    return EXIT_FAILURE;
  }

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
  std::cout << "Polymer: " << polymer << "\n";
  std::cout << "Length: " << polymer.size() << "\n";
  return EXIT_SUCCESS;
}
