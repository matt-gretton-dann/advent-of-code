#include <iostream>
#include <string>

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to parse line\n";
    return 1;
  }

  unsigned score{0};
  unsigned depth{0};
  bool skip{false};
  bool in_garbage{false};

  for (auto c : line) {
    if (skip) {
      skip = false;
    }
    else if (in_garbage && c == '!') {
      skip = true;
    }
    else if (in_garbage && c == '>') {
      in_garbage = false;
    }
    else if (!in_garbage && c == '{') {
      ++depth;
      score += depth;
    }
    else if (!in_garbage && c == '}') {
      --depth;
    }
    else if (!in_garbage && c == '<') {
      in_garbage = true;
    }
  }

  std::cout << "Score = " << score << '\n';
  return 0;
}
