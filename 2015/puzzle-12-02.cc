#include <cassert>
#include <cctype>
#include <functional>
#include <iostream>
#include <string>

auto do_parse(std::string const& s, std::string::size_type& pos) -> int
{
  int result = 0;
  bool ignore = false;

  while (pos < s.size()) {
    if (s[pos] == '{' || s[pos] == '[') {
      ++pos;
      result += do_parse(s, pos);
    }
    else if (s[pos] == '}') {
      ++pos;
      return ignore ? 0 : result;
    }
    else if (s[pos] == ']') {
      ++pos;
      return result;
    }
    else if (s[pos] == '"') {
      ++pos;
      auto e = s.find('"', pos);
      assert(e != std::string::npos);
      std::string v = s.substr(pos, e - pos);
      if (v == "red") {
        ignore = true;
      }
      pos = e + 1;
    }
    else if (std::isdigit(s[pos]) == 1 || s[pos] == '-') {
      std::size_t len = 0;
      result += std::stoi(s.substr(pos), &len);
      pos += len;
    }
    else {
      assert(s[pos] == ',' || s[pos] == ':');
      ++pos;
    }
  }

  return result;
}

auto parse_numbers(std::string const& s) -> int
{
  std::string::size_type pos = 0;
  int result = do_parse(s, pos);
  assert(pos == s.size());
  return result;
}

auto main() -> int
{
  int acc = 0;
  for (std::string line; std::getline(std::cin, line);) {
    acc += parse_numbers(line);
  }

  std::cout << "Accumulated value: " << acc << "\n";
  return 0;
}