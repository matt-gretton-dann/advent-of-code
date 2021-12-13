#include <iostream>
#include <string>

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to parse line\n";
    return 1;
  }

  std::string h1{line.substr(0, line.size() / 2)};
  std::string h2{line.substr(line.size() / 2)};

  std::uint64_t v{0};
  for (std::size_t idx{0}; idx < h1.size(); ++idx) {
    if (h1[idx] == h2[idx]) {
      v += (line[idx] - '0') * 2;
    }
  }

  std::cout << "CAPTCHA: " << v << '\n';
}
