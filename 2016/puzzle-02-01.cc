//
// Created by Matthew Gretton-Dann on 03/12/2021.
//
#include <array>
#include <iostream>
#include <string>

int main()
{
  constexpr std::array<unsigned, 10> down{0, 4, 5, 6, 7, 8, 9, 7, 8, 9};
  constexpr std::array<unsigned, 10> left{0, 1, 1, 2, 4, 4, 5, 7, 7, 8};
  constexpr std::array<unsigned, 10> up{0, 1, 2, 3, 1, 2, 3, 4, 5, 6};
  constexpr std::array<unsigned, 10> right{0, 2, 3, 3, 5, 6, 6, 8, 9, 9};
  unsigned pos{5};
  std::string line;
  std::string result;
  while (std::getline(std::cin, line)) {
    for (auto c : line) {
      switch (c) {
      case 'L':
        pos = left[pos];
        break;
      case 'D':
        pos = down[pos];
        break;
      case 'R':
        pos = right[pos];
        break;
      case 'U':
        pos = up[pos];
        break;
      default:
        abort();
      }
    }
    result += static_cast<char>('0' + pos);
  }
  std::cout << result << '\n';
}