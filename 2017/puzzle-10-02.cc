#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using UInt = unsigned char;
using String = std::vector<UInt>;

auto knot(String const& in, UInt current_position, UInt length) -> String
{
  String rot_left;
  rot_left.reserve(in.size());
  std::copy(in.begin() + current_position, in.end(), std::back_inserter(rot_left));
  std::copy(in.begin(), in.begin() + current_position, std::back_inserter(rot_left));

  std::reverse(rot_left.begin(), rot_left.begin() + length);

  String rot_right;
  std::copy(rot_left.end() - current_position, rot_left.end(), std::back_inserter(rot_right));
  std::copy(rot_left.begin(), rot_left.end() - current_position, std::back_inserter(rot_right));
  return rot_right;
}

auto main() -> int
{
  std::vector<UInt> string;
  string.resize(256);
  std::iota(string.begin(), string.end(), 0);

  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return 1;
  }

  /* Put the lengths into a vector.  */
  std::vector<UInt> lengths;
  lengths.reserve(line.size() + 5);
  for (auto c : line) {
    lengths.emplace_back(static_cast<UInt>(c));
  }
  lengths.emplace_back(17);
  lengths.emplace_back(31);
  lengths.emplace_back(73);
  lengths.emplace_back(47);
  lengths.emplace_back(23);

  /* Run the hash */
  UInt current_pos{0};
  UInt skip{0};
  for (unsigned i{0}; i < 64; ++i) {
    for (auto length : lengths) {
      string = knot(string, current_pos, length);
      current_pos += length + skip;
      ++skip;
    }
  }

  /* Print the dense hash.  */
  std::cout << "Dense hash: ";
  unsigned hash{0};
  for (unsigned i{0}; i < string.size(); ++i) {
    hash ^= string[i];
    if (i % 16 == 15) {
      std::cout << std::hex << std::setw(2) << std::setfill('0') << hash;
      hash = 0;
    }
  }
  std::cout << '\n';

  return 0;
}
