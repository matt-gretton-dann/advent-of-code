#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <stack>
#include <string>
#include <string_view>
#include <vector>

using Int = std::int64_t;
using NumVec = std::vector<Int>;

template<typename It>
void read_nums(It it, std::string_view const nums)
{
  char const* pos{nums.data()};
  while (pos - nums.data() < nums.size()) {
    char* next_pos{nullptr};
    *it++ = std::strtoll(pos, &next_pos, 10);
    pos = next_pos;
  }
}

constexpr auto is_zero(Int const num) -> bool { return num == 0; }

auto main() -> int try {
  std::string line;
  Int sum{0};

  while (std::getline(std::cin, line)) {
    std::stack<NumVec> diffs;
    diffs.emplace();
    read_nums(std::back_inserter(diffs.top()), line);

    while (!std::all_of(diffs.top().begin(), diffs.top().end(), is_zero)) {
      NumVec diff;
      auto const& top = diffs.top();
      for (auto it = top.begin() + 1; it != top.end(); ++it) {
        diff.emplace_back(*it - *(it - 1));
      }
      diffs.push(diff);
    }

    Int next{0};
    while (!diffs.empty()) {
      auto const& top = diffs.top();
      next += top.back();
      diffs.pop();
    }

    std::cout << line << ": " << next << '\n';
    sum += next;
  }

  std::cout << "Sum: " << sum;

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
