#include <bit>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using Byte = std::uint8_t;
using KnotString = std::vector<Byte>;

auto knot_round(KnotString const& in, Byte current_position, Byte length) -> KnotString
{
  KnotString rot_left;
  rot_left.reserve(in.size());
  std::copy(in.begin() + current_position, in.end(), std::back_inserter(rot_left));
  std::copy(in.begin(), in.begin() + current_position, std::back_inserter(rot_left));

  std::reverse(rot_left.begin(), rot_left.begin() + length);

  KnotString rot_right;
  std::copy(rot_left.end() - current_position, rot_left.end(), std::back_inserter(rot_right));
  std::copy(rot_left.begin(), rot_left.end() - current_position, std::back_inserter(rot_right));
  return rot_right;
}

using Knot = std::pair<std::uint64_t, std::uint64_t>;

auto knot(std::string const& rounds) -> Knot
{
  /* Put the lengths into a vector.  */
  std::vector<Byte> lengths;
  lengths.reserve(rounds.size() + 5);
  for (auto c : rounds) {
    lengths.emplace_back(static_cast<Byte>(c));
  }
  lengths.emplace_back(17);
  lengths.emplace_back(31);
  lengths.emplace_back(73);
  lengths.emplace_back(47);
  lengths.emplace_back(23);

  /* Run the hash */
  Byte current_pos{0};
  Byte skip{0};
  std::vector<Byte> string;
  string.resize(256);
  std::iota(string.begin(), string.end(), 0);

  for (unsigned i{0}; i < 64; ++i) {
    for (auto length : lengths) {
      string = knot_round(string, current_pos, length);
      current_pos += length + skip;
      ++skip;
    }
  }

  /* Print the dense hash.  */
  std::uint64_t result0{0};
  std::uint64_t result1{0};
  std::uint64_t* hash{&result0};
  for (unsigned i{0}; i < string.size(); ++i) {
    if (i % 16 == 0) {
      if (i == string.size() / 2) {
        hash = &result1;
      }
      *hash <<= 8;
    }
    *hash ^= string[i];
  }

  return {result0, result1};
}

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read Line\n";
    return 1;
  }

  unsigned bit_count{0};
  for (unsigned i = 0; i < 128; ++i) {
    std::string key{line};
    key += '-';
    key += std::to_string(i);
    auto [knot1, knot2] = knot(key);
    bit_count += std::popcount(knot1) + std::popcount(knot2);
  }

  std::cout << "Population count " << bit_count << '\n';
  return 0;
}
