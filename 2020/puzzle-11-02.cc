#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

using Array = std::vector<std::string>;

unsigned search(Array const& a, std::size_t row, std::size_t col, int rd, int cd)
{
  int cr = (int)row;
  int cc = (int)col;
  while (true) {
    cr += rd;
    cc += cd;
    if (cr == row && cc == col) {
      return 0;
    }
    if (cr < 0 || cr >= a.size()) {
      return 0;
    }
    if (cc < 0 || cc >= a[cr].size()) {
      return 0;
    }
    if (a[cr][cc] == '#') {
      return 1;
    }
    if (a[cr][cc] == 'L') {
      return 0;
    }
  }

  assert(false);
}

char next_state(Array const& a, std::size_t row, std::size_t col)
{
  unsigned occupied = 0;
  for (int row_delta = -1; row_delta < 2; ++row_delta) {
    for (int col_delta = -1; col_delta < 2; ++col_delta) {
      occupied += search(a, row, col, row_delta, col_delta);
    }
  }

  if (a[row][col] == 'L' && occupied == 0) {
    return '#';
  }
  else if (a[row][col] == '#' && occupied >= 5) {
    return 'L';
  }
  else {
    return a[row][col];
  }
}

unsigned long count_occupied(Array const& a)
{
  unsigned long count = 0;
  for (auto const& row : a) {
    for (auto col : row) {
      count += (col == '#');
    }
  }

  return count;
}

unsigned long run_to_steady_state(Array const& a)
{
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
int main(int argc, char** argv)
{
  Array array;

  for (std::string line; std::getline(std::cin, line);) {
    array.push_back(line);
    std::cout << line << "\n";
  }

  unsigned long result = run_to_steady_state(array);

  std::cout << "Number of occupied states: " << result << "\n";
  return 0;
}