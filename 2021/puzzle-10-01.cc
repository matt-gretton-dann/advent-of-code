//
// Created by Matthew Gretton-Dann on 10/12/2021.
//

#include <cassert>
#include <iostream>
#include <map>
#include <stack>
#include <string>

auto main() -> int
{
  std::map<char, char> matches{{'(', ')'}, {'[', ']'}, {'{', '}'}, {'<', '>'}};
  std::map<char, unsigned> scores{{')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}};

  std::string line;
  unsigned score{0};
  while (std::getline(std::cin, line)) {
    std::stack<char> expecting;
    for (auto c : line) {
      if (auto it = matches.find(c); it != matches.end()) {
        expecting.push(it->second);
      }
      else if (!expecting.empty()) {
        if (c == expecting.top()) {
          expecting.pop();
        }
        else {
          auto score_it{scores.find(c)};
          assert(score_it != scores.end());
          score += score_it->second;
          break;
        }
      }
    }
  }

  std::cout << "Total score: " << score << '\n';
  return 0;
}
