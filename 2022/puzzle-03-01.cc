//
// Created by Matthew Gretton-Dann on 02/12/2022.
//

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

using Int = std::int64_t;

auto main() -> int
{
  std::string letters{" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};

  Int score{0};
  std::string line;
  while (std::getline(std::cin, line)) {
    auto left{line.substr(0, line.size() / 2)};
    auto right{line.substr(line.size() / 2)};
    std::sort(left.begin(), left.end());
        std::sort(right.begin(), right.end());
    std::string result;
    std::set_intersection(left.begin(), left.end(), right.begin(), right.end(),
                          std::back_inserter(result));
    assert(!result.empty());
    score += letters.find(result[0]);
  }
  std::cout << "Score: " << score << '\n';
  return EXIT_SUCCESS;
}
