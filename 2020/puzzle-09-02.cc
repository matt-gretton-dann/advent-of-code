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
    bool result = true;
    auto size = buf_.size();
    if (size >= L) {
      result = valid(i);
    }
    buf_.push_back(i);
    return result;
  }

  unsigned long get_weakness(unsigned long v) {
    std::size_t sum_begin = 0;
    std::size_t sum_end = 0;
    unsigned long sum = 0;
    while (sum != v) {
      if (sum < v) {
        assert(sum_end != buf_.size());
        sum += buf_[sum_end];
        ++sum_end;
      }
      if (sum > v) {
        assert(sum_begin != buf_.size());
        sum -= buf_[sum_begin];
        ++sum_begin;
      }
    }
    std::cout << "sum_begin = " << sum_begin << " sum_end = " << sum_end
              << "\n";
    std::vector<unsigned long> vec(buf_.begin() + sum_begin,
                                   buf_.begin() + sum_end);
    std::sort(vec.begin(), vec.end());
    return vec[0] + vec[vec.size() - 1];
  }

private:
  bool valid(unsigned long v) const {
    assert(buf_.size() >= L);
    for (auto i = buf_.size() - 25; i != buf_.size(); ++i) {
      for (auto j = i + 1; j != buf_.size(); ++j) {
        if (v == buf_[i] + buf_[j]) {
          return true;
        }
      }
    }
    return false;
  }

  std::vector<unsigned long> buf_;
};

int main(int argc, char **argv) {
  Buffer<25> buf;
  unsigned long invalid = 0;
  for (std::string line; std::getline(std::cin, line);) {
    if (!buf.add_value(line)) {
      if (invalid == 0) {
        invalid = std::stoul(line);
      }
    }
  }

  std::cout << "First Invalid: " << invalid << "\n";
  std::cout << "Weakness: " << buf.get_weakness(invalid) << "\n";
  return 0;
}