#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  std::vector<int> vals;
  for (std::string line; std::getline(std::cin, line);) {
    int val = std::stoi(line, nullptr, 10);
    auto size = vals.size();
    for (std::size_t i = 0; i < size; ++i) {
      int64_t v1 = vals[i];
      for (std::size_t j = 0; j < i; ++j) {
        int64_t v2 = vals[j];
        if (v1 + v2 + val == 2020) {
          std::cout << v1 << " * " << v2 << " * " << val << " = "
                    << v1 * v2 * std::int64_t(val) << "\n";
        }
      }
    }
    vals.push_back(val);
  }

  return 0;
}