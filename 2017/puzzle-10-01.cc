#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using UInt = unsigned long;
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

  std::size_t idx{0};
  UInt current_pos{0};
  UInt skip{0};
  while (idx < line.size()) {
    auto next{line.find(',', idx)};
    if (next == std::string::npos) {
      next = line.size();
    }
    auto len{next - idx};
    auto length{std::stoul(line.substr(idx, len))};
    string = knot(string, current_pos, length);
    current_pos += length + skip;
    current_pos %= string.size();
    ++skip;
    idx += len + 1;
  }

  std::cout << string[0] << " * " << string[1] << " = " << string[0] * string[1] << '\n';
  return 0;
}
