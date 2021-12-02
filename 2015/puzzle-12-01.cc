#include <functional>
#include <iostream>
#include <regex>
#include <string>

auto parse_numbers(std::string const& s) -> int
{
  static const std::regex re("-?\\d+");
  std::string left = s;
  std::smatch m;
  int acc = 0;
  while (std::regex_search(left, m, re)) {
    acc += std::stoi(m.str(0));
    left = m.suffix();
  }

  return acc;
}

auto main() -> int
{
  int acc = 0;
  for (std::string line; std::getline(std::cin, line);) {
    acc += parse_numbers(line);
  }

  std::cout << "Accumulated value: " << acc << "\n";
  return 0;
}