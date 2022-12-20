//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <array>
#include <iostream>
#include <numeric>
#include <regex>
#include <stdexcept>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;
using Ints = std::vector<std::pair<Int, Int>>;

using namespace std::string_literals;

auto find_at_offset(Ints const& nums, Int offset) -> Int
{
  auto it{std::find_if(nums.begin(), nums.end(),[](auto const& a) { return a.first == 0;})};
  while (offset > 0) {
    --offset;
    if (it == nums.end()) {
      it = nums.begin();
    }
    ++it;
  }
  while (offset < 0) {
    ++offset;
    if (it == nums.begin()) {
      it = nums.end();
    }
    --it;
  }
  return it->first;
}

auto print_nums(auto const& v) -> void
{
  bool first{true};
  for (auto const& e : v) {
    if (!first) {
      std::cout << ", ";
    }
    else {
      first = false;
    }
    std::cout << e;
  }
  std::cout << "\n";
}

auto main() -> int
{
  std::string line;
  Ints distances;
  while (std::getline(std::cin, line)) {
    distances.emplace_back(std::make_pair(std::stoll(line),static_cast<Int>(distances.size())));
  }

  Ints nums{distances};
  std::cout << "Number of numbers: " << nums.size() << "\n";
  for (auto i : distances) {
    auto it{std::find(nums.begin(), nums.end(), i)};
    auto old_pos{it - nums.begin()};
    auto dist = i;
    auto new_pos{(old_pos + nums.size() + dist.first) % nums.size()};
    it = nums.erase(it);
    while (dist.first > 0) {
      --dist.first;
      if (it == nums.end()) {
        it = nums.begin();
      }
      ++it;
    }
    while (dist.first < 0) {
      ++dist.first;
      if (it == nums.begin()) {
        it = nums.end();
      }
      --it;
    }
    nums.insert(it, i);
    std::cout << i.first << ": was at " << old_pos << " now at "
              << std::find(nums.begin(), nums.end(), dist) - nums.begin() << "\n";
    //    std::cout << dist << ": ";
    //    print_nums(nums);
  }

  Int num1000{find_at_offset(nums, 1000)};
  Int num2000{find_at_offset(nums, 2000)};
  Int num3000{find_at_offset(nums, 3000)};
  Int result{num1000 + num2000 + num3000};
  std::cout << "Offset 1000: " << num1000 << "\n";
  std::cout << "Offset 2000: " << num2000 << "\n";
  std::cout << "Offset 3000: " << num3000 << "\n";
  std::cout << "Total: " << result << "\n";

  return EXIT_SUCCESS;
}
