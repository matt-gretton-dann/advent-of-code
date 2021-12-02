#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

// This code is horrible - my only excuse is I wrote it before breakfast

using Node = std::string;
using Nodes = std::map<Node, bool>;
using Edge = std::pair<Node, Node>;
using Weight = unsigned;
using EdgeWeights = std::map<Edge, Weight>;
using NodeQueue = std::list<Node>;

std::ostream& operator<<(std::ostream& os, NodeQueue const& q)
{
  std::string p = "";
  os << "[";
  for (auto const& n : q) {
    os << p << n;
    p = ", ";
  }
  os << "]";
  return os;
}

struct BagGraph
{
  BagGraph() { nodes_.insert({"", false}); }

  void add_edges(std::string const& s)
  {
    static const auto prefix_re = std::regex("([a-z ]+) bags? contain ");
    static const auto suffix_re = std::regex("(\\d)+ ([a-z ]+) bags?[,.]\\s*");
    static const auto empty_str = std::string("no other bags.");
    std::smatch prefix_m;
    if (!std::regex_search(s, prefix_m, prefix_re)) {
      std::cout << "Failed to match: " << s << "\n";
      assert(false);
      return;
    }

    std::string n2 = prefix_m.str(1);
    nodes_.insert({n2, false});
    std::string suffix = prefix_m.suffix();
    while (!suffix.empty()) {
      if (suffix == empty_str) {
        suffix = "";
        Edge e = std::make_pair("", n2);
        edges_.insert({e, 0});
        std::cout << "<none> -> " << n2 << ": 0\n";
      }
      else {
        std::smatch suffix_m;
        if (!std::regex_search(suffix, suffix_m, suffix_re)) {
          std::cout << "Failed to match: " << suffix << "\n";
          assert(false);
          return;
        }
        Weight w = std::stoul(suffix_m.str(1));
        Node n1 = suffix_m.str(2);
        suffix = suffix_m.suffix();
        Edge e = std::make_pair(n1, n2);
        nodes_.insert({n1, false});
        edges_.insert({e, w});
        std::cout << n1 << " -> " << n2 << ": " << w << "\n";
      }
    }
  }

  unsigned num_containers(Node const& n)
  {
    NodeQueue queue;
    // Initial population of queue:
    populate_queue(queue, n);
    std::cout << "Initial queue: " << queue << "\n";
    unsigned visited = 0;

    // Iterate over queue finding how
    while (!queue.empty()) {
      Node n1 = queue.front();
      std::cout << "Processing: " << n1 << "\n";
      queue.pop_front();
      if (!nodes_[n1]) {
        nodes_[n1] = true;
        populate_queue(queue, n1);
        ++visited;
        std::cout << "Updated queue: " << queue << "\n";
      }
      else {
        std::cout << "Already visited\n";
      }
    }

    return visited;
  }

private:
  void populate_queue(NodeQueue& queue, Node const& n)
  {
    for (auto const& kv : edges_) {
      if (kv.first.first == n && !nodes_[kv.first.second]) {
        queue.push_back(kv.first.second);
      }
    }
  }

  EdgeWeights edges_;
  Nodes nodes_;
};

int main(int argc, char** argv)
{
  BagGraph bg;
  for (std::string line; std::getline(std::cin, line);) {
    bg.add_edges(line);
  }

  Node n = "shiny gold";
  auto visited = bg.num_containers(n);
  std::cout << "Number of " << n << " containers " << visited << "\n";

  return 0;
}