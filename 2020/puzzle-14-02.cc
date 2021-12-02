#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

using Data = std::uint64_t;
using Mem = std::map<Data, Data>;

struct Computer
{
  void add_instruction(std::string const& s)
  {
    static const std::regex re("mem\\[(\\d+)\\] = (\\d+)");
    std::smatch m;

    if (s.substr(0, 4) == "mask") {
      std::string mask = s.substr(7);
      std::reverse(mask.begin(), mask.end());
      set_ = 0;
      bits_.clear();
      std::size_t bit = 0;
      std::cout << "Bitmask: floating bits: ";
      for (auto c : mask) {
        if (c == '1') {
          set_ |= Data(1) << bit;
        }
        else if (c == 'X') {
          bits_.push_back(std::size_t(1) << bit);
          std::cout << bit << " (" << (std::size_t(1) << bit) << ") ";
        }
        else {
          assert(c == '0');
        }
        ++bit;
      }
      assert(bit == mask.size());
      std::cout << " set: 0x" << std::hex << set_ << std::dec << "\n";
    }
    else if (std::regex_search(s, m, re)) {
      Data loc = std::stoul(m.str(1));
      Data value = std::stoul(m.str(2));
      set_mem(loc, value);
    }
  }

  void set_mem(Data loc, Data value)
  {
    std::cout << "Writing: " << value << " to base loc " << std::hex << loc << " which floats to:";
    for (auto b : bits_) {
      loc &= ~b;
    }
    for (std::size_t i = 0; i < (1 << bits_.size()); ++i) {
      auto l = loc | set_;
      for (std::size_t j = 0; j < bits_.size(); ++j) {
        if (i & (std::size_t(1) << j)) {
          l |= bits_[j];
        }
      }
      std::cout << " " << l;
      auto [it, success] = mem_.insert({l, value});
      if (!success) {
        it->second = value;
      }
    }
    std::cout << std::dec << "\n";
  }

  Data mem_sum() const noexcept
  {
    Data r = 0;
    for (auto v : mem_) {
      r += v.second;
    }
    return r;
  }

  Data set_;
  std::vector<std::size_t> bits_;
  Mem mem_;
};

int main(int argc, char** argv)
{
  Computer c;

  std::string line;
  while (std::getline(std::cin, line)) {
    c.add_instruction(line);
  }

  std::cout << "Memory sum: " << std::dec << c.mem_sum() << "\n";
  return 0;
}