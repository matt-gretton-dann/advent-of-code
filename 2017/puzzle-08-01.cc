#include <iostream>
#include <regex>
#include <string>

using Int = long;

enum class Action { inc, dec };

auto to_action(std::string const& s) -> Action
{
  if (s == "inc") {
    return Action::inc;
  }
  if (s == "dec") {
    return Action::dec;
  }
  abort();
}

enum class Comparison {
  less_than,
  less_than_equal,
  equal,
  not_equal,
  greater_than,
  greater_than_equal
};

auto to_comparison(std::string const& s) -> Comparison
{
  if (s == "<") {
    return Comparison::less_than;
  }
  if (s == "<=") {
    return Comparison::less_than_equal;
  }
  if (s == "==") {
    return Comparison::equal;
  }
  if (s == "!=") {
    return Comparison::not_equal;
  }
  if (s == ">") {
    return Comparison::greater_than;
  }
  if (s == ">=") {
    return Comparison::greater_than_equal;
  }
  abort();
}

auto compare(Int l, Comparison c, Int r) -> bool
{
  switch (c) {
  case Comparison::less_than:
    return l < r;
  case Comparison::less_than_equal:
    return l <= r;
  case Comparison::equal:
    return l == r;
  case Comparison::not_equal:
    return l != r;
  case Comparison::greater_than_equal:
    return l >= r;
  case Comparison::greater_than:
    return l > r;
  }
  abort();
}

auto main() -> int
{
  static std::regex cmd_re{"([a-z]+) (...) (-?\\d+) if ([a-z]+) (.+) (-?\\d+)"};

  std::map<std::string, Int> registers;
  std::string line;

  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (!std::regex_search(line, m, cmd_re)) {
      std::cerr << "Unable to match: " << line << '\n';
      return 1;
    }

    std::string dest_reg{m.str(1)};
    Action act{to_action(m.str(2))};
    Int delta{std::stol(m.str(3))};
    std::string cmp_reg{m.str(4)};
    Comparison cmp{to_comparison(m.str(5))};
    Int cmp_value{std::stol(m.str(6))};

    auto cmp_it{registers.find(cmp_reg)};
    bool cmp_pass{compare(cmp_it == registers.end() ? 0 : cmp_it->second, cmp, cmp_value)};

    if (cmp_pass) {
      if (act == Action::dec) {
        delta = -delta;
      }
      auto [it, success] = registers.insert({dest_reg, delta});
      if (!success) {
        it->second += delta;
      }
    }
  }

  auto largest_it{
    std::max_element(registers.begin(), registers.end(),
                     [](auto const& l, auto const& r) { return l.second < r.second; })};

  std::cout << "Largest register: " << largest_it->first << "\nHas value: " << largest_it->second
            << '\n';
}
