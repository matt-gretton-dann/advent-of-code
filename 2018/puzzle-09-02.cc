//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <utility>

using UInt = std::uint64_t;
using Int = std::int64_t;
using State = std::list<UInt>;
using Players = std::vector<UInt>;

template<typename iterator>
auto move(iterator begin, iterator end, iterator current, Int count)
{
  while (count > 0) {
    if (current == end) {
      current = begin;
    }
    ++current;
    --count;
  }
  while (count < 0) {
    if (current == begin) {
      current = end;
    }
    --current;
    ++count;
  }
  if (current == end) {
    current = begin;
  }
  return current;
}

auto main() -> int
{
  std::string line;
  std::regex const re{"(\\d+) players; last marble is worth (\\d+) points"};

  // Read data
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return EXIT_FAILURE;
  }

  std::smatch m;
  if (!std::regex_match(line, m, re)) {
    std::cerr << "Unable to interpret: " << line << "\n";
    return EXIT_FAILURE;
  }

  UInt const player_count{std::stoull(m.str(1))};
  UInt const last_marble{std::stoull(m.str(2)) * 100};

  Players players(player_count, 0);

  State game;
  game.push_back(0);
  auto current_pos{game.begin()};

  UInt player{0};
  for (UInt marble{1}; marble <= last_marble; ++marble) {
    if (marble % 23 == 0) {
      current_pos = move(game.begin(), game.end(), current_pos, -7);
      players[player] += marble + *current_pos;
      current_pos = game.erase(current_pos);
      if (current_pos == game.end()) {
        current_pos = game.begin();
      }
    }
    else {
      current_pos = move(game.begin(), game.end(), current_pos, 2);
      current_pos = game.insert(current_pos, marble);
    }
    player = (player + 1) % player_count;
  }

  auto max_elt{std::max_element(players.begin(), players.end())};
  std::cout << "high score is: " << *max_elt << "\n";
  return EXIT_SUCCESS;
}
