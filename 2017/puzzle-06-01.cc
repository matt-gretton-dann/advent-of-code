#include <iostream>
#include <string>
#include <vector>

using UInt = unsigned long;
using LoadingVector = std::vector<UInt>;

auto split(std::string const& line, char c) -> LoadingVector
{
  LoadingVector result;
  std::size_t idx{0};
  while (idx != std::string::npos) {
    auto next{line.find(c, idx)};
    if (next == std::string::npos) {
      result.push_back(std::stoul(line.substr(idx)));
      idx = next;
    }
    else {
      result.push_back(std::stoul(line.substr(idx, next - idx)));
      idx = next + 1;
    }
  }
  return result;
}

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable ot read line\n";
    return 1;
  }

  LoadingVector load{split(line, '\t')};

  std::vector<LoadingVector> states;

  do {
    states.push_back(load);
    auto it{std::max_element(load.begin(), load.end())};
    auto amt{*it};
    *it = 0;
    for (auto& it2 : load) {
      it2 += amt / load.size();
    }
    amt %= load.size();
    while (amt != 0) {
      ++it;
      if (it == load.end()) {
        it = load.begin();
      }
      ++*it;
      --amt;
    }
  } while (std::find(states.begin(), states.end(), load) == states.end());
  std::cout << "Count: " << states.size() << '\n';
  return 0;
}
