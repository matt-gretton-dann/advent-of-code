#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

template <std::size_t L> struct Buffer {
  Buffer() {}

  bool add_value(std::string const &s) {
    unsigned long i = std::stoul(s);
    auto size = buf_.size();
    if (size == L && !valid(i)) {
      return false;
    }
    if (size == L) {
      buf_.pop_front();
    }
    buf_.push_back(i);
    return true;
  }

private:
  bool valid(unsigned long v) const {
    assert(buf_.size() == L);
    for (auto i = buf_.begin(); i != buf_.end(); ++i) {
      auto j = i;
      for (++j; j != buf_.end(); ++j) {
        if (v == *i + *j) {
          std::cout << "v = "
                    << " " << *i << " + " << *j << "\n";
          return true;
        }
      }
    }
    return false;
  }

  std::list<unsigned long> buf_;
};

int main(int argc, char **argv) {
  Buffer<25> buf;
  for (std::string line; std::getline(std::cin, line);) {
    if (!buf.add_value(line)) {
      std::cout << "Invalid: " << line << "\n";
      return 0;
    }
  }
  return 0;
}