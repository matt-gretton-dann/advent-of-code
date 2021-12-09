//
// Created by Matthew Gretton-Dann on 03/12/2021.
//

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>

using LetterCountPair = std::pair<char, unsigned>;

/**
 * \brief Ordering for LetterCount pairs
 *
 * This takes LetterCount pairs and orders them as defined in the puzzle.  So higher counts come
 * first, followed by letters in alphabetical order.
 */
struct LetterCountOrder
{
  constexpr auto operator()(LetterCountPair const& lhs, LetterCountPair const& rhs) const noexcept
    -> bool
  {
    if (lhs.second > rhs.second) {
      return true;
    }
    if (lhs.second == rhs.second) {
      return lhs.first < rhs.first;
    }
    return false;
  }
};

auto main() -> int
{
  std::string line;
  std::uint64_t sum{0};

  while (std::getline(std::cin, line)) {
    std::map<char, unsigned> letter_counts;
    auto it{line.begin()};
    /* Iterate over the first part which we use to get the character counts.  */
    while (std::isdigit(*it) == 0 && it != line.end()) {
      if (*it == '-') {
        it++;
        continue;
      }
      /* Insert or update the letter count for *it.  */
      auto [lcit, success] = letter_counts.insert({*it, 1});
      if (!success) {
        lcit->second++;
      }
      ++it;
    }

    /* Read the sector number.  */
    assert(it != line.end());
    std::size_t idx{0};
    std::uint64_t sector_num{std::stoul(line.substr(it - line.begin()), &idx)};
    it += static_cast<std::ptrdiff_t>(idx);
    assert(*it == '[');
    ++it;

    /* ordered_letters is the set of letter_counts ordered so that starting with begin() we get the
     * expected letters.
     */
    std::set<LetterCountPair, LetterCountOrder> ordered_letters;
    for (auto lcit : letter_counts) {
      ordered_letters.insert(lcit);
    }

    /* Check that the checksum is what we want.  */
    bool success = true;
    auto olit = ordered_letters.begin();
    while (success && *it != ']' && it != line.end()) {
      if (olit->first != *it) {
        success = false;
      }
      ++it;
      ++olit;
    }

    /* If success add to the sector sum.  */
    if (success) {
      sum += sector_num;
    }
  }

  std::cout << "Sum of sector numbers: " << sum << '\n';
  return 0;
}
