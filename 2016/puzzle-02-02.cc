//
// Created by Matthew Gretton-Dann on 03/12/2021.
//
#include <array>
#include <iostream>
#include <string>

int main()
{
  constexpr std::array<unsigned, 14> down{0, 3, 6, 7, 8, 5, 10, 11, 12, 9, 10, 13, 12, 13};
  constexpr std::array<unsigned, 14> left{0, 1, 2, 2, 3, 5, 5, 6, 7, 8, 10, 10, 11, 13};
  constexpr std::array<unsigned, 14> up{0, 1, 2, 1, 4, 5, 2, 3, 4, 9, 6, 7, 8, 11};
  constexpr std::array<unsigned, 14> right{0, 1, 3, 4, 4, 6, 7, 8, 9, 9, 11, 12, 12, 13};
  const std::string_view letters{"0123456789ABCD"};
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
    result += letters[pos];
  }
  std::cout << result << '\n';
}