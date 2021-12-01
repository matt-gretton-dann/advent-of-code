#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

struct Cups {
  Cups(std::string const &s) : s_(s) {
    std::cout << "Initial state: " << s_ << "\n";
  }

  void play(unsigned moves) {
    // We assume the current value is always at index 0.
    // So the values to remove are in offsets [1, 4).
    while (moves-- > 0) {
      char current = s_[0];
      char d = s_[0] == '1' ? '9' : s_[0] - 1;
      std::string::size_type dest = std::string::npos;
      while (dest == std::string::npos) {
        dest = s_.find(d, 4);
        d = d == '1' ? '9' : d - 1;
      }

      s_ = s_.substr(4, dest - 3) + s_.substr(1, 3) + s_.substr(dest + 1) +
           s_[0];
      std::cout << moves << ": " << s_ << "\n";
    }
  }

  std::string result() const {
    auto split = s_.find('1');
    return s_.substr(split + 1) + s_.substr(0, split);
  }

private:
  std::string s_;
};

int main(void) {
  std::string line;
  std::getline(std::cin, line);
  Cups cups(line);
  cups.play(100);
  std::cout << "Result: " << cups.result() << "\n";
  return 0;
}