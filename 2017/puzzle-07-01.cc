#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

using UInt = unsigned long;

auto main() -> int
{
  static std::regex top_re{R"(^([a-z]+) \((\d+)\)$)"};
  static std::regex node_re{R"(^([a-z]+) \((\d+)\) -> (.+)$)"};

  std::string line;
  std::map<std::string, UInt> weights;
  std::set<std::string> children_nodes;
  std::set<std::string> maybe_root;

  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (std::regex_search(line, m, node_re)) {
      auto weight{std::stoul(m.str(2))};
      auto node{m.str(1)};
      auto children{m.str(3)};
      weights.insert({node, weight});
      maybe_root.insert(node);
      std::size_t idx{0};
      while (idx < children.size()) {
        if (children[idx] == ' ' || children[idx] == ',') {
          ++idx;
          continue;
        }

        std::string child;
        while (idx < children.size() && children[idx] != ',') {
          child.push_back(children[idx]);
          ++idx;
        }
        children_nodes.insert(child);
      }
    }
    else if (std::regex_search(line, m, top_re)) {
      auto weight{std::stoul(m.str(2))};
      auto const& node{m.str(1)};
      weights.insert({node, weight});
      children_nodes.insert(node);
    }
  }

  std::set<std::string> root;
  std::set_difference(maybe_root.begin(), maybe_root.end(), children_nodes.begin(),
                      children_nodes.end(), std::inserter(root, root.end()));
  std::string prefix = "Root = ";
  for (auto const& n : root) {
    std::cout << prefix << n;
    prefix = ", ";
  }
  std::cout << '\n';
}
