//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <utility>

using UInt = std::uint64_t;
using State = std::vector<UInt>;
using Players = std::vector<UInt>;

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
  UInt const last_marble{std::stoull(m.str(2))};

  Players players(player_count, 0);

  State game;
  game.reserve(last_marble);
  game.push_back(0);
  UInt current_pos{0};

  UInt player{0};
  for (UInt marble{1}; marble <= last_marble; ++marble) {
    if (marble % 23 == 0) {
      players[player] += marble;
      current_pos = (current_pos + game.size() - 7) % game.size();
      players[player] += game.at(current_pos);
      game.erase(game.begin() + static_cast<State::difference_type>(current_pos));
      current_pos %= game.size();
    }
    else {
      current_pos = (current_pos + 2) % game.size();
      game.insert(game.begin() + static_cast<State::difference_type>(current_pos), marble);
    }
    player = (player + 1) % player_count;
  }

  auto max_elt{std::max_element(players.begin(), players.end())};
  std::cout << "high score is: " << *max_elt << "\n";
  return EXIT_SUCCESS;
}
