//
// Created by Matthew Gretton-Dann on 10/12/2021.
//

#include <cassert>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

auto main() -> int
{
  std::map<char, char> matches{{'(', ')'}, {'[', ']'}, {'{', '}'}, {'<', '>'}};
  std::map<char, unsigned> scores{{')', 1}, {']', 2}, {'}', 3}, {'>', 4}};

  std::string line;
  std::vector<std::uint64_t> line_scores;
  while (std::getline(std::cin, line)) {
    bool incomplete{true};
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
          incomplete = false;
          break;
        }
      }
      else {
        abort();
      }
    }

    if (!incomplete) {
      continue;
    }

    // expecting has the characters we expect.
    std::uint64_t score{0};
    while (!expecting.empty()) {
      score *= 5;
      auto score_it{scores.find(expecting.top())};
      assert(score_it != scores.end());
      score += score_it->second;
      expecting.pop();
    }
    line_scores.push_back(score);
  }

  std::sort(line_scores.begin(), line_scores.end());
  std::cout << "Middle score: " << line_scores.at(line_scores.size() / 2) << '\n';
  return 0;
}
