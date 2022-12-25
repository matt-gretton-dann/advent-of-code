//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <array>
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std::string_literals;

using Int = std::int64_t;
using UInt = std::uint64_t;

auto to_base10(std::string const& s) -> Int
{
  Int num{0};
  for (auto c : s) {
    num *= 5;
    switch (c) {
    case '=':
      num -= 2;
      break;
    case '-':
      num -= 1;
      break;
    case '0':
      break;
    case '1':
      num += 1;
      break;
    case '2':
      num += 2;
      break;
    default:
      abort();
    }
  }
  return num;
}

auto to_base5(Int num) -> std::string
{
  bool const negate{num < 0};
  if (negate) {
    num = -num;
  }

  std::string result;
  while (num != 0) {
    Int digit = num % 5;
    num /= 5;
    switch (digit) {
    case 0:
    case 1:
    case 2:
      result.push_back(static_cast<char>('0' + digit));
      break;
    case 3:
      result.push_back('=');
      num += 1;
      break;
    case 4:
      result.push_back('-');
      num += 1;
      break;
    default:
      abort();
    }
  }
  std::reverse(result.begin(), result.end());

  if (negate) {
    for (auto& c : result) {
      switch (c) {
      case '=':
        c = '2';
        break;
      case '-':
        c = '1';
        break;
      case '0':
        c = '0';
        break;
      case '1':
        c = '-';
        break;
      case '2':
        c = '=';
        break;
      default:
        abort();
      }
    }
  }

  return result;
}

auto main() -> int
{
  std::string line;
  Int total{0};

  while (std::getline(std::cin, line)) {
    Int const base10{to_base10(line)};
    total += base10;
    std::cout << line << " = " << base10 << "\n";
  }

  std::string const base5{to_base5(total)};
  std::cout << "Total: " << total << " = " << base5 << "\n";
  return EXIT_SUCCESS;
}
