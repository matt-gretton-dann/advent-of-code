#include <algorithm>
#include <iostream>
#include <string>

auto main() -> int
{
  std::string line;
  std::uint64_t checksum{0};
  while (std::getline(std::cin, line) && !line.empty()) {
    std::vector<std::uint64_t> nums;
    std::size_t idx{0};
    while (idx < line.size()) {
      if (std::iswspace(line[idx]) == 1) {
        ++idx;
        continue;
      }

      std::size_t end{0};
      nums.push_back(std::stoul(line.substr(idx), &end));
      idx += end;
    }

    for (auto n1 : nums) {
      for (auto n2 : nums) {
        if (n1 != n2 && n1 % n2 == 0)
          checksum += n1 / n2;
      }
    }
  }

  std::cout << "Checksum: " << checksum << '\n';
  return 0;
}
