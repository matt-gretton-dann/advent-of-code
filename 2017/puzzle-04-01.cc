#include <iostream>
#include <string>
#include <vector>

auto split(std::string const& line, char c) -> std::vector<std::string>
{
  std::vector<std::string> result;
  std::size_t idx{0};
  while (idx != std::string::npos) {
    auto next{line.find(c, idx)};
    if (next == std::string::npos) {
      result.push_back(line.substr(idx));
      idx = next;
    }
    else {
      result.push_back(line.substr(idx, next - idx));
      idx = next + 1;
    }
  }
  return result;
}

auto main() -> int
{
  std::string line;
  unsigned valid{0};
  while (std::getline(std::cin, line) && !line.empty()) {
    std::vector<std::string> words{split(line, ' ')};
    std::sort(words.begin(), words.end());
    if (std::adjacent_find(words.begin(), words.end()) == words.end()) {
      ++valid;
    }
  }

  std::cout << "Valid passphrases: " << valid << '\n';
  return 0;
}
