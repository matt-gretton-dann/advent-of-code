//
// Created by Matthew Gretton-Dann on 09/12/2022.
//

#include <array>
#include <iostream>
#include <set>
#include <string>
#include <utility>

using Int = std::int64_t;
using UInt = std::uint64_t;

struct Ordering
{
  auto operator()(std::string const& left, std::string const& right) const noexcept -> bool
  {
    auto left_it{left.begin()};
    auto right_it{right.begin()};
    UInt left_depth{0};
    UInt right_depth{0};

    while (left_it != left.end() && right_it != right.end()) {
      while (left_it != left.end() && *left_it == '[') {
        ++left_depth;
        ++left_it;
      }
      while (right_it != right.end() && *right_it == '[') {
        ++right_depth;
        ++right_it;
      }
      std::size_t left_pos{0};
      std::size_t right_pos{0};
      UInt left_num{0};
      UInt right_num{0};
      if (left_it != left.end() && std::isdigit(*left_it) != 0) {
        left_num = std::stoull(left.substr(left_it - left.begin()), &left_pos);
      }
      if (right_it != right.end() && std::isdigit(*right_it) != 0) {
        right_num = std::stoull(right.substr(right_it - right.begin()), &right_pos);
      }
      left_it += static_cast<std::string::difference_type>(left_pos);
      right_it += static_cast<std::string::difference_type>(right_pos);
      if (right_pos == 0) {
        if (left_pos != 0) {
          return false;
        }
        if (left_depth < right_depth) {
          return true;
        }
        if (right_depth > left_depth) {
          return false;
        }
      }
      else if (left_pos == 0) {
        return true;
      }
      if (right_num < left_num) {
        return false;
      }
      if (left_num < right_num) {
        return true;
      }

      while (left_it != left.end() && *left_it == ']') {
        --left_depth;
        ++left_it;
      }
      while (right_it != right.end() && *right_it == ']') {
        --right_depth;
        ++right_it;
      }
      if (left_depth != right_depth) {
        return left_depth < right_depth;
      }
      if (left_it != left.end() && *left_it == ',') {
        ++left_it;
      }
      if (right_it != left.end() && *right_it == ',') {
        ++right_it;
      }
    }

    return left_it == left.end();
  }
};

auto main() -> int
{
  std::string line;
  std::set<std::string, Ordering> strings;
  strings.insert("[[2]]");
  strings.insert("[[6]]");

  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    strings.insert(line);
  }
  UInt dist2{0};
  UInt dist6{0};
  UInt idx{1};
  for (auto const& s : strings) {
    if (Ordering{}(s, "[[2]]")) {
      ++dist2;
      ++dist6;
    }
    else if (Ordering{}(s, "[[6]]")) {
      ++dist6;
    }
    std::cout << idx++ << ": " << s << '\n';
  }
  std::cout << "Score: " << dist2 << " * " << dist6 << " = " << dist2 * dist6 << '\n';
  return 0;
}
