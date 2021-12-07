#include <iostream>
#include <regex>
#include <string>

auto swap_pos(std::string s, std::size_t a, std::size_t b) -> std::string
{
  std::swap(s.at(a), s.at(b));
  return s;
}

auto swap_letters(std::string const& s, char a, char b) -> std::string
{
  std::string result;
  std::transform(s.begin(), s.end(), std::back_inserter(result), [a, b](char c) {
    if (c == a) {
      return b;
    }
    if (c == b) {
      return a;
    }
    return c;
  });
  return result;
}

auto rotate_left(std::string const& s, std::size_t amt) -> std::string
{
  amt %= s.size();
  return (s + s).substr(amt, s.size());
}

auto rotate_right(std::string const& s, std::size_t amt) -> std::string
{
  amt %= s.size();
  return (s + s).substr(s.size() - amt, s.size());
}

auto rotate_on_index(std::string const& s, char c) -> std::string
{
  auto idx{s.find(c)};
  std::size_t amt{1 + idx + (idx >= 4 ? 1 : 0)};
  return rotate_right(s, amt);
}

auto reverse_str(std::string s, std::size_t a, std::size_t b) -> std::string
{
  std::reverse(s.begin() + a, s.begin() + b + 1);
  return s;
}
auto move(std::string s, std::size_t a, std::size_t b) -> std::string
{
  auto c{s.at(a)};
  s.erase(a, 1);
  s.insert(s.begin() + b, c);
  return s;
}

auto main() -> int
{
  static std::regex swap_pos_re{"swap position (\\d+) with position (\\d+)"};
  static std::regex swap_letter_re{"swap letter (.) with letter (.)"};
  static std::regex rotate_left_re{"rotate left (\\d+) steps?"};
  static std::regex rotate_right_re{"rotate right (\\d+) steps?"};
  static std::regex rotate_based_re{"rotate based on position of letter (.)"};
  static std::regex reverse_re{"reverse positions (\\d+) through (\\d+)"};
  static std::regex move_re{"move position (\\d+) to position (\\d+)"};

  std::string password{"abcdefgh"};
  std::string line;
  std::smatch m;
  while (std::getline(std::cin, line)) {
    if (std::regex_search(line, m, swap_pos_re)) {
      password = swap_pos(password, std::stoul(m.str(1)), std::stoul(m.str(2)));
    }
    else if (std::regex_search(line, m, swap_letter_re)) {
      password = swap_letters(password, m.str(1)[0], m.str(2)[0]);
    }
    else if (std::regex_search(line, m, rotate_left_re)) {
      password = rotate_left(password, std::stoul(m.str(1)));
    }
    else if (std::regex_search(line, m, rotate_right_re)) {
      password = rotate_right(password, std::stoul(m.str(1)));
    }
    else if (std::regex_search(line, m, rotate_based_re)) {
      password = rotate_on_index(password, m.str(1)[0]);
    }
    else if (std::regex_search(line, m, reverse_re)) {
      password = reverse_str(password, std::stoul(m.str(1)), std::stoul(m.str(2)));
    }
    else if (std::regex_search(line, m, move_re)) {
      password = move(password, std::stoul(m.str(1)), std::stoul(m.str(2)));
    }
    else {
      std::cerr << "Unable to interpret " << line << "\n";
      return 1;
    }
    std::cout << line << " -> " << password << '\n';
  }
  std::cout << password << '\n';
}