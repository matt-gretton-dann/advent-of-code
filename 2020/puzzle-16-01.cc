#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

using ValidVector = std::vector<bool>;

struct Classifier {
  void add_ranges(std::string const &s) {
    static const std::regex re("(\\w+): (\\d+)-(\\d+) or (\\d+)-(\\d+)");
    std::smatch m;
    if (std::regex_search(s, m, re)) {
      std::cout << m.str(1) << "valid = ";
      add_range(m.str(2), m.str(3));
      std::cout << ", ";
      add_range(m.str(4), m.str(5));
      std::cout << "\n";
    }
  }

  void add_range(std::string const &lows, std::string const &highs) {
    int low = std::stoi(lows);
    int high = std::stoi(highs);
    ++high;
    if (vecs_.size() < high) {
      vecs_.resize(high, false);
    }
    std::cout << "[" << low << ", " << high << "]";
    while (low < high) {
      vecs_[low] = true;
      ++low;
    }
  }

  int count_errors(std::string const &s) {
    std::string::size_type pos = 0;
    int result = 0;
    while (pos < s.size()) {
      std::size_t len = 0;
      int num = std::stoi(s.substr(pos), &len);
      if (!vecs_[num]) {
        result += num;
        std::cout << "Invalid: " << num << "\n";
      }
      pos += len;
      while (pos < s.size() && s[pos] == ',') {
        ++pos;
      }
    }

    return result;
  }
  ValidVector vecs_;
};

enum class State { Permitted, Your, Nearby };

int main(int argc, char **argv) {
  std::string line;

  State s = State::Permitted;
  Classifier c;
  int e = 0;
  while (std::getline(std::cin, line)) {
    if (line == "your ticket:") {
      s = State::Your;
    } else if (line == "nearby tickets:") {
      s = State::Nearby;
    } else if (line == "") {
      /* nothing */;
    } else if (s == State::Permitted) {
      c.add_ranges(line);
    } else if (s == State::Nearby) {
      e += c.count_errors(line);
    }
  }

  std::cout << "Error count: " << e << "\n";
  return 0;
}