//
// Created by mgretton on 10/01/2022.
//

#ifndef ADVENT_OF_CODE_GRAPH_UTILS_H
#define ADVENT_OF_CODE_GRAPH_UTILS_H

#include <algorithm>
#include <cassert>
#include <limits>
#include <map>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <utility>

/** \brief  Implement Dijkstra's algorithm.
 *
 * @tparam Cost               Cost type
 * @tparam Node               Node class
 * @tparam TransitionManager  Class type that manages state transitions
 * @tparam FinishedFn         Function type to call to test whether a node matches finished state
 * @param  initial            Initial node
 * @param  initial_cost       Cost of initial node
 * @param  transition_manager Object to manage state transitions
 * @return                    Node, Cost pair indicating success.  On failure default node with
 *                            maximum cost is returned.
 *
 * \a Cost Must be a numeric type.  \a Node must be a class which is default constructable, be less
 * than comparable.
 *
 * \a transition_manager must be an object with the following public interfaces:
 * \code
 *   auto is_finished(Node const& node) -> bool;
 *
 *   template <typename Inserter>
 *   void generate_children(Node const& node, Inserter inserter);
 * \endcode
 *
 * The \c generate_children() method is called whenever we examine the node \c node, and should
 * call \c inserter(next_node, cost_delta) for every node directly reachable from \c node.  Where
 * \c next_node is the next node to visit and \c cost_delta is the incremental cost to visit the new
 * node.  \c inserter should be called for all possible nodes that are visitable, it will ensure
 * that duplicates are managed correctly/
 *
 * The \c is_finished() method should return true if the given node is in a finished state.
 */
template<typename Cost, typename Node, typename TransitionManager>
auto dijkstra(Node const& initial, Cost initial_cost, TransitionManager transition_manager)
  -> std::pair<Node, Cost>
{
  /** Helper struct to order node pointers. */
  struct NodePCmp
  {
    auto operator()(Node const* lhs, Node const* rhs) const noexcept -> bool
    {
      if (lhs == nullptr && rhs != nullptr) {
        return true;
      }
      if (rhs == nullptr) {
        return false;
      }
      return *lhs < *rhs;
    }

    auto operator()(Node const* lhs, Node const& rhs) const noexcept -> bool
    {
      if (lhs == nullptr) {
        return true;
      }
      return *lhs < rhs;
    }

    auto operator()(Node const& lhs, Node const* rhs) const noexcept -> bool
    {
      if (rhs == nullptr) {
        return false;
      }
      return lhs < *rhs;
    }
  };

  /* We maintain two maps - one from node to cost and the other from cost to nodes.  */
  /** \c nodes maintains a map of all the nodes we've visited or want to visit.  Nodes that cost
   * less than the current cost at the front of costs have been visited.  The rest haven't.
   */
  std::map<Node const*, Cost, NodePCmp> nodes;
  /** \c costs maintains a map of costs to the nodes that cost that much to visit. */
  std::map<Cost, std::unordered_set<Node const*>> costs;
  Cost current_cost{initial_cost};
  Node const* current_node{new Node(initial)};
  nodes.insert({current_node, current_cost});
  costs.insert({current_cost, {current_node}});

  /* Helper lambda to clean up after ourselves.  */
  auto cleanup = [](auto& nodes) {
    for (auto const& it : nodes) {
      delete it.first;
    }
  };

  /* Helper lambda to insert into the maps.  */
  auto inserter = [&costs, &nodes, &current_cost](Node const& node, Cost cost) {
    cost += current_cost;
    auto node_it{nodes.find(&node)};
    /* Skip inserting nodes we've already visited, or that would cost more to visit.  */
    if (node_it != nodes.end() && node_it->second <= cost) {
      return;
    }

    Node const* nodep{nullptr};
    if (node_it == nodes.end()) {
      nodep = new Node(node);
      nodes.insert({nodep, cost});
    }
    else {
      /* Node has a cheaper cost than we thought: Remove the node from its old cost list */
      nodep = node_it->first;
      auto cost_it{costs.find(node_it->second)};
      assert(cost_it != costs.end());
      cost_it->second.erase(nodep);

      /* Now update the cost in the nodes map and use the nodep as the node pointer.  */
      node_it->second = cost;
    }

    auto [cost_it, success] = costs.insert({cost, {}});
    cost_it->second.insert(nodep);
  };

  std::uint64_t iter{0};
  while (!costs.empty()) {
    auto cost_it{costs.begin()};
    current_cost = cost_it->first;
    assert(iter < nodes.size());
    assert(std::accumulate(costs.begin(), costs.end(), std::size_t{0}, [](auto a, auto c) {
      return a + c.second.size();
      }) == nodes.size() - iter);
    for (auto& nodep : cost_it->second) {
      if (iter++ % 100'000 == 0) {
        std::cout << "Iteration: " << iter << " cost " << current_cost
          << " total number of nodes: " << nodes.size()
          << ", number of costs left to visit: " << costs.size()
          << ", number of nodes left: " << (nodes.size() - iter) << '\n';
      }
      if (transition_manager.is_finished(*nodep)) {
        auto result{std::make_pair(*nodep, current_cost)};
        cleanup(nodes);
        return result;
      }
      transition_manager.generate_children(*nodep, inserter);
    }
    costs.erase(costs.begin());
  }

  cleanup(nodes);
  return {Node{}, std::numeric_limits<Cost>::max()};
}

#endif  // ADVENT_OF_CODE_GRAPH_UTILS_H
