#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

// Merge two strings of answers, preserving order and ensuring uniqueness of
// each answer.
std::string merge_answers(std::string const &a, std::string const &b) {
  std::string result;
  std::string::size_type pos_a = 0;
  std::string::size_type pos_b = 0;
  while (pos_a < a.length() && pos_b < b.length()) {
    if (a[pos_a] < b[pos_b]) {
      result += a[pos_a];
      ++pos_a;
    } else if (b[pos_b] < a[pos_a]) {
      result += b[pos_b];
      ++pos_b;
    } else {
      assert(a[pos_a] == b[pos_b]);
      result += a[pos_a];
      ++pos_a;
      ++pos_b;
    }
  }
  result += a.substr(pos_a);
  result += b.substr(pos_b);
  std::cout << "Merged " << a << " and " << b << " to " << result << "\n";
  return result;
}

int main(int argc, char **argv) {

  std::string current_answers;
  unsigned count = 0;
  for (std::string line; std::getline(std::cin, line);) {
    if (!line.empty()) {
      std::sort(line.begin(), line.end());
      current_answers = merge_answers(current_answers, line);
    } else {
      std::cout << "Length of " << current_answers << " = "
                << current_answers.length() << "\n";
      count += current_answers.length();
      current_answers.clear();
    }
  }
  std::cout << "Length of " << current_answers << " = "
            << current_answers.length() << "\n";
  count += current_answers.length();

  std::cout << "Total length = " << count << "\n";
  return 0;
}