//
// Created by Matthew Gretton-Dann on 02/12/2022.
//

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

using Int = std::int64_t;

auto main() -> int
{
  std::string letters{" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};

  Int score{0};
  std::string line;
  Int state{0};
  std::string current;
  while (std::getline(std::cin, line)) {
    std::sort(line.begin(), line.end());
    if (state == 0) {
      current = line;
      ++state;
    }
    else if (state == 1) {
      std::string r;
      std::set_intersection(current.begin(), current.end(), line.begin(), line.end(),
                            std::back_inserter(r));
      current = r;
      ++state;
    }
    else if (state == 2) {
      std::string r;
      std::set_intersection(current.begin(), current.end(), line.begin(), line.end(),
                            std::back_inserter(r));
      state = 0;
      assert(!r.empty());
      score += letters.find(r[0]);
    }
  }
  assert(state == 0);
  std::cout << "Score: " << score << '\n';
  return EXIT_SUCCESS;
}
