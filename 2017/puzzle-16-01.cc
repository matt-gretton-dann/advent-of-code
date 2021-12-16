#include <cassert>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using UInt = unsigned long;

auto spin(std::string const& s, UInt num) -> std::string
{
  assert(num < s.size());

  std::string result;
  result.reserve(s.size());
  auto inserter{std::back_inserter(result)};
  std::copy(s.end() - num, s.end(), inserter);
  std::copy(s.begin(), s.end() - num, inserter);
  return result;
}

auto exchange(std::string const& s, UInt a, UInt b) -> std::string
{
  assert(a < s.size());
  assert(b < s.size());

  std::string result{s};
  std::swap(result[a], result[b]);
  return result;
}

auto partner(std::string const& s, char a, char b) -> std::string
{
  std::string result;
  std::transform(s.begin(), s.end(), std::back_inserter(result), [a, b](char c) {
    if (c == a) {
      return b;
    }
    if (c == b) {
      return a;
    }
    return c;
  });

  return result;
}

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return 1;
  }

  std::string programs;
  programs.resize(16);
  std::iota(programs.begin(), programs.end(), 'a');

  std::size_t idx{0};
  while (idx < line.size()) {
    switch (line[idx++]) {
    case ',':
      break;
    case 's': {
      std::size_t end{0};
      auto x{std::stoul(line.substr(idx), &end)};
      idx += end;
      programs = spin(programs, x);
      break;
    }
    case 'x': {
      std::size_t end{0};
      auto x{std::stoul(line.substr(idx), &end)};
      idx += end;
      if (line[idx++] != '/') {
        std::cerr << "Missing exchange / at " << idx - 1 << ": " << line.substr(idx - 1) << '\n';
      }
      auto y{std::stoul(line.substr(idx), &end)};
      idx += end;
      programs = exchange(programs, x, y);
      break;
    }
    case 'p': {
      auto a{line[idx++]};
      if (line[idx++] != '/') {
        std::cerr << "Missing partner / at " << idx - 1 << ": " << line.substr(idx - 1) << '\n';
      }
      auto b{line[idx++]};
      programs = partner(programs, a, b);
      break;
    }
    default:
      std::cerr << "Unable to understand at index " << idx - 1 << ": " << line.substr(idx - 1)
                << '\n';
      abort();
      break;
    }
  }

  std::cout << "Result: " << programs << '\n';
  return 0;
}
