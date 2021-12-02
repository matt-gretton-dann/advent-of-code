#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct PasswordChecker
{
  PasswordChecker(std::string const& s)
  {
    std::size_t pos = 0;
    min_ = std::stoul(s, &pos, 10);
    assert(s[pos] == '-');
    std::string s2 = s.substr(pos + 1);
    max_ = std::stoul(s2, &pos, 10);
    assert(s2[pos] == ' ');
    c_ = s2[pos + 1];
    assert(s2[pos + 2] == ':');
    assert(s2[pos + 3] == ' ');
    password_ = s2.substr(pos + 4);
  }

  bool is_valid() const
  {
    std::string::size_type count = 0;
    for (auto c : password_) {
      if (c == c_) {
        ++count;
      }
    }

    return (count >= min_) && (count <= max_);
  }

  std::string::size_type min_;
  std::string::size_type max_;
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