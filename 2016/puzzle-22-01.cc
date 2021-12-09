#include <iostream>
#include <regex>
#include <string>
#include <vector>

struct Node
{
  unsigned long x_;
  unsigned long y_;
  unsigned long size_;
  unsigned long used_;
  unsigned long avail_;
  unsigned long percent_;
};

using NodeVector = std::vector<Node>;

auto main() -> int
{
  static std::regex header_re{R"(^Filesystem\s+Size\s+Used\s+Avail\s+Use%$)"};
  static std::regex header2_re{"^root@"};
  static std::regex info_re{
    R"(^/dev/grid/node-x(\d+)-y(\d+)\s+(\d+)T\s+(\d+)T\s+(\d+)T\s+(\d+)%$)"};

  std::string line;
  NodeVector nodes;
  std::smatch m;
  while (std::getline(std::cin, line)) {
    if (std::regex_search(line, m, header_re)) {
      continue;
    }
    if (std::regex_search(line, m, header2_re)) {
      continue;
    }
    if (std::regex_search(line, m, info_re)) {
      nodes.push_back({std::stoul(m.str(1)), std::stoul(m.str(2)), std::stoul(m.str(3)),
                       std::stoul(m.str(4)), std::stoul(m.str(5)), std::stoul(m.str(6))});
    }
    else {
      std::cerr << "Unable to interpret: " << line << '\n';
      return 1;
    }
  }

  unsigned viable{0};
  for (auto it1{nodes.begin()}; it1 != nodes.end(); ++it1) {
    if (it1->used_ == 0) {
      continue;
    }
    for (auto it2{nodes.begin()}; it2 != nodes.end(); ++it2) {
      if (it2 == it1) {
        continue;
      }
      if (it1->used_ <= it2->avail_) {
        ++viable;
      }
    }
  }

  std::cout << "Viable " << viable << '\n';
  return 0;
}