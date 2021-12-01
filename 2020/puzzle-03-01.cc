#include <cassert>
#include <iostream>
#include <string>
#include <vector>

std::string rotate(std::string const &s, std::string::size_type amount) {
  auto length = s.length();
  return (s + s).substr(amount % length, length);
}

int main(int argc, char **argv) {
  unsigned count = 0;
  std::string::size_type rotation = 0;
  for (std::string line; std::getline(std::cin, line);) {
    auto rotated = rotate(line, rotation);
    rotation += 3;
    count += (rotated[0] == '#');
  }

  std::cout << "Number of trees: " << count << "\n";
  return 0;
}