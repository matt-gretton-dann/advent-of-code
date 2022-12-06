//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

using UInt = std::uint64_t;

auto main() -> int
{
  std::string line;
  unsigned const match_len{14};
  while (std::getline(std::cin, line) && !line.empty()) {
    std::bitset<UCHAR_MAX> bits;
    for (UInt i{0}; i != line.size(); ++i) {
      bits.flip(static_cast<unsigned char>(line[i]));
      if (i >= match_len) {
        bits.flip(static_cast<unsigned char>(line[i - match_len]));
      }
      if (i >= (match_len - 1) && bits.count() == match_len) {
        std::cout << "First match at " << i + 1 << '\n';
        break;
      }
    }
  }
  return 0;
}
