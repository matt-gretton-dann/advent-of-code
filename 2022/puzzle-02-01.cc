//
// Created by Matthew Gretton-Dann on 02/12/2022.
//

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

using Int = std::int64_t;

auto main() -> int
{
  // A = Rock, B = Paper, C = Scissors
  // X = Rock, B = Paper, C = Scisscors
  std::map<std::string, Int> scores = {
    {"A X", 1 + 3}, {"A Y", 2 + 6}, {"A Z", 3 + 0}, {"B X", 1 + 0}, {"B Y", 2 + 3},
    {"B Z", 3 + 6}, {"C X", 1 + 6}, {"C Y", 2 + 0}, {"C Z", 3 + 3},
  };

  Int score{0};
  std::string line;
  while (std::getline(std::cin, line)) {
    auto it{scores.find(line)};
    assert(it != scores.end());
    score += it->second;
  }
  std::cout << "Score: " << score << '\n';
  return EXIT_SUCCESS;
}
