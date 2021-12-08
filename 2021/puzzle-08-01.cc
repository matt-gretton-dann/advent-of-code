//
// Created by Matthew Gretton-Dann on 08/12/2021.
//

#include <iostream>
#include <string>

auto main() -> int
{
  std::string line;
  unsigned count{0};
  while (std::getline(std::cin, line)) {
    line = line.substr(line.find('|') + 1);
    unsigned this_digit{0};
    for (auto c : line) {
      if (c == ' ') {
        if (this_digit == 2 || this_digit == 3 || this_digit == 4 || this_digit == 7) {
          ++count;
        }
        this_digit = 0;
      }
      else {
        ++this_digit;
      }
    }
    if (this_digit == 2 || this_digit == 3 || this_digit == 4 || this_digit == 7) {
      ++count;
    }
  }
  std::cout << "Simple digits: " << count << '\n';
  return 0;
}