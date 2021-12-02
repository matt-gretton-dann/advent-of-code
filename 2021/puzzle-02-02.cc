//
// Created by Matthew Gretton-Dann on 02/12/2021.
//

#include <iostream>
#include <string>

auto main() -> int
{
  using namespace std::string_literals;

  static const auto fwd{"forward "s};
  static const auto down{"down "s};
  static const auto up{"up "s}; // NOLINT(readability-identifier-length)

  std::string line;
  std::uint64_t horiz{0};
  std::uint64_t depth{0};
  std::uint64_t aim{0};
  while (std::getline(std::cin, line)) {
    if (line.size() > fwd.size() && line.substr(0, fwd.size()) == fwd) {
      auto dist{std::stoul(line.substr(fwd.size()))};
      horiz += dist;
      depth += aim * dist;
    }
    if (line.size() > down.size() && line.substr(0, down.size()) == down) {
      aim += std::stoul(line.substr(down.size()));
    }
    if (line.size() > up.size() && line.substr(0, up.size()) == up) {
      aim -= std::stoul(line.substr(up.size()));
    }
  }

  std::cout << "Distance * depth: " << horiz << " * " << depth << " = " << horiz * depth << '\n';
}