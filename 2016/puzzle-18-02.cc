//
// Created by Matthew Gretton-Dann on 07/12/2021.
//

#include <iostream>
#include <numeric>
#include <string>

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to get line.\n";
    return 1;
  }

  line = '.' + line + '.';
  unsigned safe_count{0};
  constexpr unsigned row_count{400000};
  for (unsigned row{0}; row < row_count; ++row) {
    safe_count = std::accumulate(line.begin(), line.end(), safe_count,
                                 [](unsigned a, char c) { return a + (c == '.' ? 1 : 0); }) -
                 2;
    std::string next_line{"."};
    next_line.reserve(line.size());
    for (unsigned i = 1; i < line.size() - 1; ++i) {
      auto check{line.substr(i - 1, 3)};
      if (check == "^^." || check == ".^^" || check == "^.." || check == "..^") {
        next_line += '^';
      }
      else {
        next_line += '.';
      }
    }
    next_line += '.';
    line = next_line;
  }

  std::cout << "Number of safe spots: " << safe_count << '\n';
  return 0;
}