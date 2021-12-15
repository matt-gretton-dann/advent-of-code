#include <cassert>
#include <iostream>
#include <map>
#include <string>

struct InTree
{
  constexpr auto visited() const noexcept -> bool { return visited_; }
  constexpr void visit() { visited_ = true; }

private:
  bool visited_{false};
};

using UInt = unsigned long;
using VisitMap = std::map<UInt, InTree>;
using Connections = std::multimap<UInt, UInt>;

auto find_next_node(VisitMap& map)
{
  return std::find_if(map.begin(), map.end(), [](auto const& it) { return !it.second.visited(); });
}

auto main() -> int
{
  std::string line;
  Connections connections;

  while (std::getline(std::cin, line) && !line.empty()) {
    std::size_t end{0};
    auto from{std::stoul(line, &end)};
    assert(line.substr(end, 5) == " <-> ");
    auto idx{end + 5};
    while (idx < line.size()) {
      if (line[idx] == ',' || line[idx] == ' ') {
        ++idx;
        continue;
      }

      auto to{std::stoul(line.substr(idx), &end)};
      connections.insert({from, to});
      connections.insert({to, from});
      idx += end;
    }
  }

  VisitMap connected0;
  connected0.insert({0, InTree{}});
  while (true) {
    auto it{find_next_node(connected0)};
    if (it == connected0.end()) {
      break;
    }
    it->second.visit();

    auto [it_begin, it_end] = connections.equal_range(it->first);
    for (auto it2{it_begin}; it2 != it_end; ++it2) {
      connected0.insert({it2->second, InTree{}});
    }
  }

  std::cout << "Size of connected tree: " << connected0.size() << '\n';
  return 0;
}
