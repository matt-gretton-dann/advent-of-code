#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct PasswordChecker
{
  PasswordChecker(std::string const& s)
  {
    std::size_t pos = 0;
    pos1_ = std::stoul(s, &pos, 10);
    assert(s[pos] == '-');
    std::string s2 = s.substr(pos + 1);
    pos2_ = std::stoul(s2, &pos, 10);
    assert(s2[pos] == ' ');
    c_ = s2[pos + 1];
    assert(s2[pos + 2] == ':');
    assert(s2[pos + 3] == ' ');
    password_ = s2.substr(pos + 4);
  }

  bool is_valid() const
  {
    std::string::size_type count = 0;
    if (password_.at(pos1_ - 1) == c_) {
      ++count;
    }
    if (password_.at(pos2_ - 1) == c_) {
      ++count;
    }
    return count == 1;
  }

  std::string::size_type pos1_;  // 1 based
  std::string::size_type pos2_;  // 1 based
  std::string::value_type c_;
  std::string password_;
};

int main(int argc, char** argv)
{
  unsigned count = 0;
  for (std::string line; std::getline(std::cin, line);) {
    PasswordChecker check(line);
    if (check.is_valid()) {
      ++count;
    }
    else {
      std::cout << "IN";
    }
    std::cout << "VALID: " << line << "\n";
  }

  std::cout << "Number of valid: " << count << "\n";
  return 0;
}