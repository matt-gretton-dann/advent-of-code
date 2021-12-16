#include <iomanip>
#include <iostream>
#include <regex>
#include <string>

using UInt = std::uint32_t;

auto lcg(UInt prev, UInt factor) -> UInt
{
  auto t{static_cast<std::uint64_t>(prev) * static_cast<std::uint64_t>(factor)};
  return static_cast<UInt>(t % 2147483647ULL);
}

auto lcgA(UInt prev) -> UInt
{
  do {
    prev = lcg(prev, 16807);
  } while (prev % 4 != 0);
  return prev;
}

auto lcgB(UInt prev) -> UInt
{
  do {
    prev = lcg(prev, 48271);
  } while (prev % 8 != 0);
  return prev;
}

auto main() -> int
{
  static std::regex a_re{"Generator A starts with (\\d+)"};
  static std::regex b_re{"Generator B starts with (\\d+)"};
  UInt prevA{0};
  UInt prevB{0};
  std::string line;
  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (std::regex_search(line, m, a_re)) {
      prevA = std::stoul(m.str(1));
    }
    else if (std::regex_search(line, m, b_re)) {
      prevB = std::stoul(m.str(1));
    }
    else {
      std::cerr << "Unable to interpret: " << line << '\n';
      return 1;
    }
  }

  UInt judged{0};
  for (UInt i = 0; i < 5'000'000; ++i) {
    prevA = lcgA(prevA);
    prevB = lcgB(prevB);
    if (((prevA ^ prevB) & 0xffffULL) == 0) {
      ++judged;
    }
  }

  std::cout << "Matched " << judged << '\n';
}
