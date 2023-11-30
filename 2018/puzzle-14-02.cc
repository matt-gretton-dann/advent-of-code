//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <regex>
#include <utility>
#include <cassert>

using Int = std::int64_t;
using UInt = std::uint64_t;
using Point = std::pair<Int, Int>;

auto main() -> int
{
  std::string search;
  if (!std::getline(std::cin, search)) {
    std::cerr << "Unable to read line.\n";
    return EXIT_FAILURE;
  }
  std::string scores{"37"};
  UInt elf1_pos{0};
  UInt elf2_pos{1};

  UInt rounds{1'000'000};
  while (true) {
    while (scores.size() < rounds) {
      Int const new_score{scores[elf1_pos] - '0' + scores[elf2_pos] - '0'};
      assert(new_score < 100);
      if (new_score >= 10) {
        scores.push_back(static_cast<char>('0' + (new_score / 10)));
      }
      scores.push_back(static_cast<char>('0' + (new_score % 10)));
      elf1_pos = (elf1_pos + 1 + scores[elf1_pos] - '0') % scores.size();
      elf2_pos = (elf2_pos + 1 + scores[elf2_pos] - '0') % scores.size();
    }
    rounds += 1'000'000;

    auto it{scores.find(search)};
    if (it != std::string::npos) {
      std::cout << "Search string appears after: " << it << "\n";
      return EXIT_SUCCESS;
    }
  }
}
