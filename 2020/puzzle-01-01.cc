#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
  std::vector<int> vals;
  for (std::string line; std::getline(std::cin, line);) {
    int val = std::stoi(line, nullptr, 10);
    for (auto v : vals) {
      if (v + val == 2020) {
        std::cout << v << " * " << val << " = " << v * val << "\n";
      }
    }
    vals.push_back(val);
  }

  return 0;
}