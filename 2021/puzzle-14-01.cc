#include <iostream>
#include <map>
#include <string>

using PolymerMap = std::map<std::pair<char, char>, char>;

template<typename Fn>
void expand1(char c1, char c2, PolymerMap const& polymers, unsigned current_depth,
             unsigned max_depth, Fn action)
{
  if (current_depth == max_depth) {
    action(c1);
    return;
  }

  auto it{polymers.find({c1, c2})};
  if (it == polymers.end()) {
    action(c1);
    return;
  }
  expand1(c1, it->second, polymers, current_depth + 1, max_depth, action);
  expand1(it->second, c2, polymers, current_depth + 1, max_depth, action);
}

template<typename Fn>
void expand(std::string const& str, unsigned max_depth, PolymerMap const& polymers, Fn action)
{
  for (std::size_t idx{0}; idx < str.size() - 1; ++idx) {
    expand1(str[idx], str[idx + 1], polymers, 0, max_depth, action);
  }
  action(str.back());
}

auto main() -> int
{
  std::string initial;
  std::string line;
  PolymerMap polymers;

  if (!std::getline(std::cin, initial)) {
    std::cerr << "Unable to read initial lines.\n";
    return 1;
  }

  if (!std::getline(std::cin, line) && !line.empty()) {
    std::cerr << "Missing empty line.\n";
    return 1;
  }

  while (std::getline(std::cin, line) && !line.empty()) {
    polymers.insert({{line[0], line[1]}, line.back()});
  }

  std::map<char, unsigned> counts;
  auto action = [&counts](char c) {
    auto [it, success] = counts.insert({c, 0});
    (it->second)++;
  };

  expand(initial, 10, polymers, action);

  for (auto const& c : counts) {
    std::cout << c.first << " -> " << c.second << '\n';
  }
  auto [min, max] = std::minmax_element(
    counts.begin(), counts.end(), [](auto const& l, auto const& r) { return l.second < r.second; });
  std::cout << "Min: " << min->first << " -> " << min->second << '\n';
  std::cout << "Max: " << max->first << " -> " << max->second << '\n';
  std::cout << "Difference: " << max->second - min->second << '\n';
  return 0;
}
