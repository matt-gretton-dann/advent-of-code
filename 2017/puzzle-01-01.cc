#include <iostream>
#include <string>

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to parse line\n";
    return 1;
  }

  std::uint64_t v{0};
  for (std::size_t idx{0}; idx < line.size() - 1; ++idx) {
    if (line[idx] == line[idx + 1]) { v += line[idx] - '0';}
  }
  if (line[0] == line[line.size() - 1]) {
    v += line[0] -'0';
  }

  std::cout << "CAPTCHA: " << v << '\n';
}
