#include <iostream>
#include <string>

template<typename T>
auto sign(T i) -> decltype(i)
{
  if (i < 0) {
    return -1;
  }
  if (i > 0) {
    return 1;
  }
  return 0;
}

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Broken line" << '\n';
    return 1;
  }

  auto target{std::stoul(line)};
  std::pair<int, int> dist{1, 0};
  std::pair<int, int> pos{0, 0};
  decltype(target) val{1};

  while (val + std::abs(dist.first) + std::abs(dist.second) < target) {
    val += std::abs(dist.first) + std::abs(dist.second);
    pos = {pos.first + dist.first, pos.second + dist.second};
    dist = {-dist.second, dist.first};
    if (dist.first > 0) {
      ++dist.first;
    }
    if (dist.first < 0) {
      --dist.first;
    }
  }
  auto delta{target - val};
  pos = {pos.first + sign(dist.first) * delta, pos.second + sign(dist.second) * delta};

  std::cout << "Pos = [" << pos.first << ", " << pos.second
            << "], distance = " << std::abs(pos.first) + std::abs(pos.second) << '\n';
  return 0;
}
