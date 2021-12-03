//
// Created by Matthew Gretton-Dann on 03/12/2021.
//

#include <cassert>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

auto to_num(std::string const& s) -> std::uint64_t
{
  std::uint64_t result{0};
  for (auto c : s) {
    result <<= 1;
    result |= (c == '1' ? 1 : 0);
  }
  return result;
}

auto find_oxygen_rating(std::vector<std::string> const& lines) -> std::string
{
  std::vector<std::string> data{lines};
  assert(!data.empty());
  std::size_t len{data[1].size()};
  for (std::size_t i{0}; i < len; ++i) {
    auto one_bits{std::accumulate(
      data.begin(), data.end(), std::uint64_t{0},
      [i](std::uint64_t a, std::string const& s) { return a + (s[i] == '1' ? 1 : 0); })};
    char keep{(one_bits * 2 >= data.size()) ? '1' : '0'};
    std::cout << "Size of data = " << data.size() << " Number of one bits = " << one_bits
              << " keeping " << keep << '\n';
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

auto find_co2_rating(std::vector<std::string> const& lines) -> std::string
{
  std::vector<std::string> data{lines};
  assert(!data.empty());
  std::size_t len{data[1].size()};
  for (std::size_t i{0}; i < len; ++i) {
    auto one_bits{std::accumulate(
      data.begin(), data.end(), std::uint64_t{0},
      [i](std::uint64_t a, std::string const& s) { return a + (s[i] == '1' ? 1 : 0); })};
    char keep{(one_bits * 2 >= data.size()) ? '0' : '1'};
    std::cout << "Size of data = " << data.size() << " Number of one bits = " << one_bits
              << " keeping " << keep << '\n';
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

  auto oxygen_rating{to_num(find_oxygen_rating(lines))};
  auto co2_rating{to_num(find_co2_rating(lines))};

  std::cout << "Oxygen generator rating * CO2 scrubber rating = " << oxygen_rating << " * "
            << co2_rating << " = " << oxygen_rating * co2_rating << '\n';
  return 0;
}