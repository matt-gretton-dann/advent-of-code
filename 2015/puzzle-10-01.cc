#include <functional>
#include <iostream>
#include <string>

auto look_and_say(std::string const& s) -> std::string
{
  std::string result;
  for (std::string::size_type i = 0; i < s.length();) {
    unsigned num = 0;
    char c = s[i];
    while (s[i] == c) {
      ++i;
      ++num;
    }
    result += std::to_string(num) + c;
  }

  return result;
}

auto main() -> int
{
  for (std::string line; std::getline(std::cin, line);) {
    std::cout << "Application 0, length = " << line.length() << ": " << line << "\n";
    for (int i = 1; i < 41; ++i) {
      line = look_and_say(line);
      std::cout << "Application " << i << ", length = " << line.length() << "\n";
    }
    std::cout << "Length: " << line.length() << "\n";
  }
  return 0;
}