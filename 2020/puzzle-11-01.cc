#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

using Array = std::vector<std::string>;

char next_state(Array const &a, std::size_t row, std::size_t col) {
  unsigned occupied = 0;
  if (row > 0) {
    if (col > 0) {
      occupied += (a[row - 1][col - 1] == '#');
    }
    occupied += a[row - 1][col] == '#';
    if (col < a[row - 1].size() - 1) {
      occupied += (a[row - 1][col + 1] == '#');
    }
  }
  if (col > 0) {
    occupied += (a[row][col - 1] == '#');
  }
  if (col < a[row].size() - 1) {
    occupied += (a[row][col + 1] == '#');
  }
  if (row < a.size() - 1) {
    if (col > 0) {
      occupied += (a[row + 1][col - 1] == '#');
    }
    occupied += a[row + 1][col] == '#';
    if (col < a[row + 1].size() - 1) {
      occupied += (a[row + 1][col + 1] == '#');
    }
  }

  if (a[row][col] == 'L' && occupied == 0) {
    return '#';
  } else if (a[row][col] == '#' && occupied >= 4) {
    return 'L';
  } else {
    return a[row][col];
  }
}

unsigned long count_occupied(Array const &a) {
  unsigned long count = 0;
  for (auto const &row : a) {
    for (auto col : row) {
      count += (col == '#');
    }
  }

  return count;
}

unsigned long run_to_steady_state(Array const &a) {
  Array current = a;
  bool changed = true;
  unsigned i = 0;
  while (changed) {
    std::cout << "------ iter = " << i++ << "\n";
    changed = false;
    Array next;
    for (std::size_t row = 0; row < current.size(); ++row) {
      std::string s = "";
      for (std::size_t col = 0; col < current[row].size(); ++col) {
        char c = next_state(current, row, col);
        s.push_back(c);
        if (c != current[row][col]) {
          changed = true;
        }
      }
      next.push_back(s);
      std::cout << s << "\n";
    }
    current = next;
  }

  return count_occupied(current);
}
int main(int argc, char **argv) {
  Array array;

  for (std::string line; std::getline(std::cin, line);) {
    array.push_back(line);
    std::cout << line << "\n";
  }

  unsigned long result = run_to_steady_state(array);

  std::cout << "Number of occupied states: " << result << "\n";
  return 0;
}