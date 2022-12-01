//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cstdlib>
#include <iostream>
#include <set>
#include <string>

using Int = std::int64_t;

auto main() -> int
{
  Int current_value{0};
  std::multiset<Int> values;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      values.insert(current_value);
      current_value = 0;
    }
    else {
      Int incr{std::stoll(line)};
      current_value += incr;
    }
  }
  values.insert(current_value);

  Int top_three{0};
  auto it = values.crbegin();
  top_three += *it++;
  top_three += *it++;
  top_three += *it++;
  std::cout << "Sum of values: " << top_three << '\n';
  return EXIT_SUCCESS;
}
