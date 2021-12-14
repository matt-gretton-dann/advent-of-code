#include <cassert>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

using UInt = unsigned long;
using Nodes = std::multimap<std::string, std::string>;
using WeightMap = std::map<std::string, std::pair<UInt, UInt>>;

void find_broken(Nodes const& nodes, WeightMap& weights, std::string const& node)
{
  auto [begin_it, end_it] = nodes.equal_range(node);
  UInt count{0};
  UInt total_weight{0};
  UInt weight1{0};
  UInt weight2{0};
  std::string name1;
  std::string name2;
  for (auto it{begin_it}; it != end_it; ++it) {
    find_broken(nodes, weights, it->second);
    ++count;
    auto w{weights.find(it->second)->second.second};
    total_weight += w;
    if (weight1 == 0) {
      weight1 = w;
      name1 = it->second;
    }
    else if (w != weight1 && weight2 == 0) {
      weight2 = w;
      name2 = it->second;
    }
    else if (w != weight1 && w != weight2) {
      abort();
    }
  }

  std::string wrong_name;
  UInt wrong_weight{0};
  UInt correct_weight{0};
  if (weight2 != 0 && (weight1 * (count - 1) + weight2) == total_weight) {
    wrong_name = name2;
    wrong_weight = weight2;
    correct_weight = weight1;
  }
  if (weight2 != 0 && (weight2 * (count - 1) + weight1) == total_weight) {
    wrong_name = name1;
    wrong_weight = weight1;
    correct_weight = weight2;
  }

  if (wrong_weight != 0) {
    std::cout << "Wrong node: " << wrong_name << "\n";
    std::cout << "Total weight is " << wrong_weight << " but should be " << correct_weight << '\n';
    std::cout << "Node weight should therefore be: "
              << weights.find(wrong_name)->second.first + correct_weight - wrong_weight << '\n';
    std::exit(0);
  }

  auto it{weights.find(node)};
  assert(it != weights.end());
  it->second.second = it->second.first + total_weight;
}

auto main() -> int
{
  static std::regex top_re{R"(^([a-z]+) \((\d+)\)$)"};
  static std::regex node_re{R"(^([a-z]+) \((\d+)\) -> (.+)$)"};

  std::string line;
  WeightMap weights;
  Nodes nodes;
  std::set<std::string> children_nodes;
  std::set<std::string> maybe_root;

  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (std::regex_search(line, m, node_re)) {
      auto weight{std::stoul(m.str(2))};
      auto node{m.str(1)};
      auto children{m.str(3)};
      weights.insert({node, {weight, 0}});
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
        nodes.insert({node, child});
      }
    }
    else if (std::regex_search(line, m, top_re)) {
      auto weight{std::stoul(m.str(2))};
      auto const& node{m.str(1)};
      weights.insert({node, {weight, weight}});
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
  find_broken(nodes, weights, *root.begin());
}
