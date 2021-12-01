#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

// This code is horrible - my only excuse is I wrote it before breakfast

using Node = std::string;
using Edge = std::pair<Node, Node>;
using Weight = unsigned;
using EdgeWeights = std::map<Edge, Weight>;

struct BagGraph {
  void add_edges(std::string const &s) {
    static const auto prefix_re = std::regex("([a-z ]+) bags? contain ");
    static const auto suffix_re = std::regex("(\\d)+ ([a-z ]+) bags?[,.]\\s*");
    static const auto empty_str = std::string("no other bags.");
    std::smatch prefix_m;
    if (!std::regex_search(s, prefix_m, prefix_re)) {
      std::cout << "Failed to match: " << s << "\n";
      assert(false);
      return;
    }

    std::string n1 = prefix_m.str(1);
    std::string suffix = prefix_m.suffix();
    while (!suffix.empty()) {
      if (suffix == empty_str) {
        std::cout << n1 << " -> none: <empty>\n";
        suffix.clear();
      } else {
        std::smatch suffix_m;
        if (!std::regex_search(suffix, suffix_m, suffix_re)) {
          std::cout << "Failed to match: " << suffix << "\n";
          assert(false);
          return;
        }
        Weight w = std::stoul(suffix_m.str(1));
        Node n2 = suffix_m.str(2);
        suffix = suffix_m.suffix();
        Edge e = std::make_pair(n1, n2);
        edges_.insert({e, w});
        std::cout << n1 << " -> " << n2 << ": " << w << "\n";
      }
    }
  }

  Weight num_contained(Node const &n) {
    // Get the number of bags including the node we've been asked for so the
    // result is 1 less than the internal result
    return num_contained1(n) - 1;
  }

private:
  Weight num_contained1(Node const &n) {
    Weight w = 0;
    bool contains_something = false;
    for (auto const &kv : edges_) {
      if (kv.first.first == n) {
        w += kv.second * num_contained1(kv.first.second);
        contains_something = true;
      }
    }
    return w + 1;
  }

  EdgeWeights edges_;
};

int main(int argc, char **argv) {
  BagGraph bg;
  for (std::string line; std::getline(std::cin, line);) {
    bg.add_edges(line);
  }

  Node n = "shiny gold";
  auto contained = bg.num_contained(n);
  std::cout << n << " contains " << contained << " bags.\n";

  return 0;
}