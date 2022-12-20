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

auto offset_into(Int dist, Int size)
{
  if (dist < 0) {
    dist += size * (1 - dist / size);
  }
  dist %= size;
  return dist;
}

auto find_at_offset(Ints const& nums, Int offset) -> Int
{
  auto it{std::find_if(nums.begin(), nums.end(), [](auto const& a) { return a.first == 0; })};
  offset = offset_into(offset + (it - nums.begin()), nums.size());
  return nums[offset].first;
}

auto main() -> int
{
  std::string line;
  Ints distances;
  Int const multiplier{811589153};
  while (std::getline(std::cin, line)) {
    distances.emplace_back(
      std::make_pair(std::stoll(line) * multiplier, static_cast<Int>(distances.size())));
  }

  Ints nums{distances};
  std::cout << "Number of numbers: " << nums.size() << "\n";
  for (unsigned iter{0}; iter < 10; ++iter) {
    for (auto i : distances) {
      auto it{nums.erase(std::find(nums.begin(), nums.end(), i))};
      Int const num_size{static_cast<Int>(nums.size())};
      Int dist{offset_into(i.first + (it - nums.begin()), num_size)};
      nums.insert(nums.begin() + dist, i);
    }
  }

  Int const num1000{find_at_offset(nums, 1000)};
  Int const num2000{find_at_offset(nums, 2000)};
  Int const num3000{find_at_offset(nums, 3000)};
  Int const result{num1000 + num2000 + num3000};
  std::cout << "Offset 1000: " << num1000 << "\n";
  std::cout << "Offset 2000: " << num2000 << "\n";
  std::cout << "Offset 3000: " << num3000 << "\n";
  std::cout << "Total: " << result << "\n";

  return EXIT_SUCCESS;
}
