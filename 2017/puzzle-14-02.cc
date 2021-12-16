#include <bit>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using UInt = unsigned;
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

  std::set<std::pair<UInt, UInt>> clusters;

  for (unsigned i = 0; i < 128; ++i) {
    constexpr std::uint64_t top_bit{1ULL << 63};
    std::string key{line};
    key += '-';
    key += std::to_string(i);
    auto [knot1, knot2] = knot(key);
    for (auto j = 0; j < 64; ++j) {
      if (0 != (knot1 & top_bit)) {
        clusters.insert({j, i});
      }
      if (0 != (knot2 & top_bit)) {
        clusters.insert({j + 64, i});
      }
      knot1 <<= 1;
      knot2 <<= 1;
    }
  }

  std::cout << "Population count " << clusters.size() << '\n';

  UInt cluster_count{0};
  while (!clusters.empty()) {
    ++cluster_count;
    std::set<std::pair<UInt, UInt>> to_check;
    to_check.insert(*clusters.begin());
    while (!to_check.empty()) {
      auto it{to_check.begin()};
      auto [x, y] = *it;
      clusters.erase(*it);
      to_check.erase(it);
      if (clusters.contains({x + 1, y})) {
        to_check.insert({x + 1, y});
      }
      if (clusters.contains({x, y + 1})) {
        to_check.insert({x, y + 1});
      }
      if (x != 0 && clusters.contains({x - 1, y})) {
        to_check.insert({x - 1, y});
      }
      if (y != 0 && clusters.contains({x, y - 1})) {
        to_check.insert({x, y - 1});
      }
    }
  }

  std::cout << "Number of cluster groups " << cluster_count << '\n';
  return 0;
}
