//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cassert>
#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <regex>
#include <utility>

using Int = std::int64_t;
using UInt = std::uint64_t;
using Point = std::pair<Int, Int>;

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return EXIT_FAILURE;
  }
  UInt const rounds{std::stoull(line)};

  std::vector<uint8_t> scores;
  scores.push_back(3);
  scores.push_back(7);
  UInt elf1_pos{0};
  UInt elf2_pos{1};

  while (scores.size() < rounds + 10) {
    uint8_t const new_score{static_cast<std::uint8_t>(scores[elf1_pos] + scores[elf2_pos])};
    assert(new_score < 100);
    if (new_score >= 10) {
      scores.push_back(new_score / 10);
    }
    scores.push_back(new_score % 10);
    elf1_pos = (elf1_pos + 1 + scores[elf1_pos]) % scores.size();
    elf2_pos = (elf2_pos + 1 + scores[elf2_pos]) % scores.size();
  }

  std::cout << "Score: ";
  for (UInt idx{rounds}; idx < rounds + 10; ++idx) {
    std::cout << static_cast<UInt>(scores[idx]);
  }
  std::cout << "\n";
}
