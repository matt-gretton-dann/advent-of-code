//
// Created by Matthew Gretton-Dann on 03/12/2021.
//

#include <cassert>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

/** \brief  Convert the string s to a number
 *  @param s String of 1s and 0s to convert
 *  @return  Value of string
 */
auto to_num(std::string const& s) -> std::uint64_t
{
  std::uint64_t result{0};
  for (auto c : s) {
    assert(c == '1' || c == '0');
    result <<= 1;
    result |= (c == '1') ? 1 : 0;
  }
  return result;
}

/** \brief Find the rating for a given selector and set of lines.
 *
 * @tparam Fn       Selector function type
 * @param  lines    Lines to select from
 * @param  selector Takes bool saying whether ones are predominant, should return character to keep.
 * @return          String of selected number.
 */
template<typename Fn>
auto find_rating(std::vector<std::string> const& lines, Fn selector) -> std::string
{
  /* Algorithm description: We copy the given lines and then for each index in the strings in turn
   * we see how many 1 digits are in that index.  The function \a selector is then called to get the
   * character to keep, before erasing the strings we do not want to keep.
   */
  std::vector<std::string> data{lines};
  assert(!data.empty());
  std::size_t len{data[1].size()};
  for (std::size_t i{0}; i < len; ++i) {
    auto one_bits{std::accumulate(
      data.begin(), data.end(), std::uint64_t{0},
      [i](std::uint64_t a, std::string const& s) { return a + (s[i] == '1' ? 1 : 0); })};
    char keep{selector(one_bits * 2 >= data.size())};
    auto it{std::remove_if(data.begin(), data.end(),
                           [keep, i](std::string const& s) { return s[i] != keep; })};
    data.erase(it, data.end());
    if (data.size() <= 1) {
      break;
    }
  }
  assert(data.size() == 1);
  return data[0];
}

auto main() -> int
{
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(std::cin, line)) {
    lines.push_back(line);
  }

  auto oxygen_rating{
    to_num(find_rating(lines, [](bool mostly_ones) { return mostly_ones ? '1' : '0'; }))};
  auto co2_rating{
    to_num(find_rating(lines, [](bool mostly_ones) { return mostly_ones ? '0' : '1'; }))};

  std::cout << "Oxygen generator rating * CO2 scrubber rating = " << oxygen_rating << " * "
            << co2_rating << " = " << oxygen_rating * co2_rating << '\n';
  return 0;
}