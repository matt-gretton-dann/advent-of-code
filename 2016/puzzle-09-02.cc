//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <cassert>
#include <iostream>
#include <string>

std::size_t decompress_length(std::string const& s)
{
  std::size_t result_len{0};
  std::size_t idx{0};
  while (idx < s.size()) {
    if (s[idx] != '(') {
      auto idx2{s.find('(', idx)};
      if (idx2 == std::string::npos) {
        idx2 = s.size();
      };
      result_len += idx2 - idx;
      idx = idx2;
    }
    else {
      assert(s[idx] == '(');
      ++idx;
      std::size_t end{0};
      auto length{std::stoul(s.substr(idx), &end)};
      idx += end;
      assert(idx < s.size());
      assert(s[idx] == 'x');
      ++idx;
      auto count{std::stoul(s.substr(idx), &end)};
      idx += end;
      assert(idx < s.size());
      assert(s[idx] == ')');
      ++idx;
      assert(length <= s.size());
      assert(idx <= s.size() - length);
      result_len += count * decompress_length(s.substr(idx, length));
      idx += length;
    }
  }

  return result_len;
}

auto main() -> int
{
  std::string line;
  std::string input;

  while (std::getline(std::cin, line)) {
    input += line;
  }
  auto output{decompress_length(input)};
  std::cout << "Output length: " << output << '\n';
  return 0;
}
