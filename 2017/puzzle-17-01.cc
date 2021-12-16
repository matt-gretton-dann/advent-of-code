#include <iostream>
#include <string>
#include <vector>

using UIntVector = std::vector<unsigned>;

auto rotate(UIntVector const& vec, unsigned long amt)
{
  amt %= vec.size();
  UIntVector result;
  result.reserve(vec.size());
  auto inserter{std::back_inserter(result)};
  std::copy(vec.begin() + amt, vec.end(), inserter);
  std::copy(vec.begin(), vec.begin() + amt, inserter);
  return result;
}

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return 1;
  }

  auto step{std::stoul(line)};

  UIntVector result;
  result.push_back(0);
  for (unsigned i{1}; i < 2018; ++i) {
    result = rotate(result, step);
    result.push_back(i);
  }
  std::cout << "Result " << result.front() << '\n';
  return 0;
}
