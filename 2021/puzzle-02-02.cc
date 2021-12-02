//
// Created by Matthew Gretton-Dann on 02/12/2021.
//

#include <iostream>
#include <string>

int main()
{
  using namespace std::string_literals;

  static const auto fwd{"forward "s};
  static const auto down{"down "s};
  static const auto up{"up "s};

  std::string line;
  unsigned long horiz{0};
  unsigned long depth{0};
  unsigned long aim{0};
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