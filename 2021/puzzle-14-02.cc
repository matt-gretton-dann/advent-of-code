#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <string>

using Starter = std::pair<char, char>;
using PolymerMap = std::map<Starter, char>;

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

  /* Work out what reactions we have in the first step. */
  std::map<Starter, std::uint64_t> counts;
  for (std::size_t idx{0}; idx < initial.size() - 1; ++idx) {
    auto [it, success] = counts.insert({{initial[idx], initial[idx + 1]}, 0});
    ++(it->second);
  }

  /* Then for each of the following steps work out the reactions that would produce, and how many
   * Note we don't care about the order they're applied here.
   */
  for (unsigned i = 0; i < 40; ++i) {
    std::map<Starter, std::uint64_t> updated_counts;
    auto a{std::accumulate(counts.begin(), counts.end(), std::uint64_t{0},
                           [](auto a, auto const& it) { return a + it.second; })};
    std::cout << "Step " << i << " size " << a << '\n';
    for (auto const& c : counts) {
      auto it{polymers.find(c.first)};
      if (it == polymers.end()) {
        auto [it2, success] = updated_counts.insert({c.first, 0});
        it2->second += c.second;
      }
      else {
        auto [it3, success3] = updated_counts.insert({{c.first.first, it->second}, 0});
        it3->second += c.second;
        auto [it4, success4] = updated_counts.insert({{it->second, c.first.second}, 0});
        it4->second += c.second;
      }
    }
    std::swap(counts, updated_counts);
  }

  /* Now count up have many for each letter we have.  We only count the first letter of each
   * molecule as the second letter will form the start of the next molecule.  Well except for the
   * last element.
   */
  std::map<char, std::uint64_t> letter_counts;
  for (auto const& c : counts) {
    auto [it, success] = letter_counts.insert({c.first.first, 0});
    it->second += c.second;
  }
  /* So add the last letter.  */
  auto [it, success] = letter_counts.insert({initial.back(), 0});
  ++(it->second);

  for (auto const& c : counts) {
    std::cout << c.first.first << c.first.second << " -> " << c.second << '\n';
  }
  for (auto const& c : letter_counts) {
    std::cout << c.first << " -> " << c.second << '\n';
  }
  auto [min, max] =
    std::minmax_element(letter_counts.begin(), letter_counts.end(),
                        [](auto const& l, auto const& r) { return l.second < r.second; });
  std::cout << "Min: " << min->first << " -> " << min->second << '\n';
  std::cout << "Max: " << max->first << " -> " << max->second << '\n';
  std::cout << "Difference: " << max->second - min->second << '\n';
  return 0;
}
