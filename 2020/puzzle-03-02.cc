#include <cassert>
#include <iostream>
#include <string>
#include <vector>

std::string rotate(std::string const &s, std::string::size_type amount) {
  auto length = s.length();
  return (s + s).substr(amount % length, length);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <right> <down>\n";
    return 1;
  }
  int right = atoi(argv[1]);
  int down = atoi(argv[2]);
  if (right <= 0) {
    std::cerr << "Right is less than 1\n";
    return 1;
  }
  if (down <= 0) {
    std::cerr << "Down is less than 1\n";
    return 1;
  }

  unsigned count = 0;
  int row = -1;
  std::string::size_type rotation = 0;
  for (std::string line; std::getline(std::cin, line);) {
    ++row;
    if ((row % down) != 0) {
      continue;
    }
    auto rotated = rotate(line, rotation);
    rotation += right;
    count += (rotated[0] == '#');
  }

  std::cout << count << "\n";
  return 0;
}