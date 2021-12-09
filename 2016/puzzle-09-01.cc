//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <cassert>
#include <iostream>
#include <string>

std::string decompress(std::string const& s)
{
  std::string result;
  std::size_t idx{0};
  while (idx < s.size()) {
    if (s[idx] != '(') {
      auto idx2{s.find('(', idx)};
      if (idx2 == std::string::npos) {
        idx2 = s.size();
      };
      result += s.substr(idx, idx2 - idx);
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
      while (count-- != 0) {
        result += s.substr(idx, length);
      }
      idx += length;
    }
  }

  return result;
}

auto main() -> int
{
  std::string line;
  std::string input;

  while (std::getline(std::cin, line)) {
    input += line;
  }
  std::cout << input;
  auto output{decompress(input)};
  std::cout << "Output length: " << output.size() << '\n';
  return 0;
}
