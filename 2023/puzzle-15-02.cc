#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using UInt = std::uint64_t;
using Lens = std::pair<std::string_view, UInt>;
using LensList = std::list<Lens>;

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
  std::vector<LensList> lenses(256);

  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read input.";
  }

  for (std::ptrdiff_t pos{0}; pos < line.size(); ++pos) {
    auto const start{pos};
    while (line[pos] != '-' && line[pos] != '=') { ++pos; }
    std::string_view const id{line.begin() + start, line.begin() + pos};
    auto const h = hash(id);
    auto& box = lenses[h];
    auto it = std::ranges::find_if(box, [id](Lens const& l) {
      return l.first == id;
    });
    if (line[pos] == '-') {
      if (it != box.end()) {
        box.erase(it);
      }
      ++pos;
    }
    else if (line[pos] == '=') {
      if (it != box.end()) {
        it->second = line[pos + 1] - '0';
      }
      else {
        box.emplace_back(id, line[pos + 1] - '0');
      }
      pos += 2;
    }
    else { std::abort(); }
  }

  UInt total{0};
  UInt box_id{1};
  for (auto const& box : lenses) {
    UInt lens_id{1};
    for (auto const [id, focal_length] : box) {
      total += box_id * lens_id * focal_length;
      ++lens_id;
    }
    ++box_id;
  }
  std::cout << "Total: " << total << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
