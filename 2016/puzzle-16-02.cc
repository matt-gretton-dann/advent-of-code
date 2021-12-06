//
// Created by Matthew Gretton-Dann on 06/12/2021.
//

#include <algorithm>
#include <iostream>
#include <string>

auto generate_data(std::string const& s) -> std::string
{
  std::string s2;
  std::string result{s};
  result.push_back('0');
  std::reverse_copy(s.begin(), s.end(), std::back_inserter(s2));
  std::transform(s2.begin(), s2.end(), std::back_inserter(result), [](char c) {
    if (c == '1') {
      return '0';
    }
    if (c == '0') {
      return '1';
    }
    abort();
  });
  return result;
}

auto checksum(std::string const& s) -> std::string
{
  std::string result;
  for (std::size_t i{0}; i < s.size(); i += 2) {
    if (s[i] == s[i + 1]) {
      result.push_back('1');
    }
    else {
      result.push_back('0');
    }
  }
  return result;
}

auto main() -> int
{
  constexpr unsigned len{35651584};
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line\n";
    return 1;
  }
  while (line.size() < len) {
    line = generate_data(line);
  }
  line.resize(len);
  while (line.size() % 2 == 0) {
    line = checksum(line);
  }
  std::cout << "Checksum " << line << '\n';
  return 0;
}