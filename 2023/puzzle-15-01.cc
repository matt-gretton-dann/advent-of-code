#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

using UInt = std::uint64_t;

auto hash(std::string_view const str) -> std::uint8_t
{
  std::uint8_t hash{0};
  for (auto const c : str) {
    hash += static_cast<std::uint8_t>(c);
    hash += hash << 4;
  }

  return hash;
}

auto main() -> int try {
  std::string line;
  UInt total_hash{0};

  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read input.";
  }

  std::size_t pos{0};
  while (pos < line.size()) {
    auto next_pos = line.find(',', pos);
    if (next_pos == std::string::npos) { next_pos = line.size(); }
    auto const h = hash(line.substr(pos, next_pos - pos));
    std::cout << line.substr(pos, next_pos - pos) << ": " << static_cast<unsigned>(h) << '\n';
    total_hash += h;
    pos = next_pos + 1;
  }

  std::cout << "Total: " << total_hash << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
