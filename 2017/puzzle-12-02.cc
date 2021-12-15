#include <cassert>
#include <iostream>
#include <map>
#include <string>

using UInt = unsigned long;
using VisitMap = std::map<UInt, bool>;
using Connections = std::multimap<UInt, UInt>;

auto find_next_node(VisitMap& map)
{
  return std::find_if(map.begin(), map.end(), [](auto const& it) { return !it.second; });
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

  UInt group_count{0};
  while (!connections.empty()) {
    VisitMap connected;
    connected.insert({connections.begin()->first, false});
    while (true) {
      auto it{find_next_node(connected)};
      if (it == connected.end()) {
        break;
      }
      it->second = true;

      auto [it_begin, it_end] = connections.equal_range(it->first);
      for (auto it2{it_begin}; it2 != it_end; ++it2) {
        connected.insert({it2->second, false});
      }
      connections.erase(it_begin, it_end);
    }
    ++group_count;
  }

  std::cout << "Number of groups: " << group_count << '\n';
  return 0;
}
