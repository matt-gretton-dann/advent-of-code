#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

using Data = std::uint64_t;
using Mem = std::vector<Data>;

struct Computer
{
  Computer() : set_(0), clear_(0), mem_() {}

  void add_instruction(std::string const& s)
  {
    static const std::regex re("mem\\[(\\d+)\\] = (\\d+)");
    std::smatch m;

    if (s.substr(0, 4) == "mask") {
      set_ = 0;
      clear_ = 0;
      for (auto c : s.substr(7)) {
        set_ <<= 1;
        clear_ <<= 1;
        set_ |= (c == '1');
        clear_ |= (c != '0');
      }
      std::cout << "mask set: " << std::hex << set_ << "\n";
      std::cout << "mask clear: " << std::hex << clear_ << "\n";
    }
    else if (std::regex_search(s, m, re)) {
      Data loc = std::stoul(m.str(1));
      Data value = std::stoul(m.str(2));
      std::cout << "mem[" << std::dec << loc << " = (" << value << " | " << set_ << ") & " << clear_
                << " = ";
      value = (value | set_) & clear_;
      std::cout << value << "\n";
      if (mem_.size() <= loc) {
        mem_.resize(loc + 1, 0);
      }
      mem_[loc] = value;
    }
  }

  Data mem_sum() const noexcept
  {
    Data r = 0;
    for (auto v : mem_) {
      r += v;
    }
    return r;
  }

  Data set_;
  Data clear_;
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