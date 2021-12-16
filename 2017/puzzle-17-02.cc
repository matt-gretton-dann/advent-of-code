#include <iostream>
#include <string>
#include <vector>

using UIntVector = std::vector<unsigned>;

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return 1;
  }

  auto step{std::stoul(line)};

  unsigned pos{0};
  unsigned after_zero{0};
  for (unsigned i{1}; i < 50'000'001; ++i) {
    /* Currently we have at position pos of a circular list of length i.
     * We want to move `step` steps round the circular list.
     */
    pos = (pos + step) % i;
    if (pos == 0) {
      after_zero = i;
    }
    /* Current position is moved on one.  */
    ++pos;
  }
  std::cout << "Result " << after_zero << '\n';
  return 0;
}
