#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using UInt = std::uint64_t;

struct Balls
{
  UInt red_{0};
  UInt green_{0};
  UInt blue_{0};
};

struct Game
{
  explicit Game(std::string input)
  {
    assert(input.starts_with("Game "));
    input = input.substr(5);
    auto pos = std::string::npos;
    id_ = std::stoul(input, &pos);
    input = input.substr(pos + 2);
    Balls balls;
    while (!input.empty()) {
      auto value = std::stoul(input, &pos);
      ++pos;
      if (input.substr(pos, 3) == "red") {
        balls.red_ += value;
        pos += 3;
      }
      else if (input.substr(pos, 4) == "blue") {
        balls.blue_ += value;
        pos += 4;
      }
      else {
        assert(input.substr(pos, 5) == "green");
        balls.green_ += value;
        pos += 5;
      }

      if (pos == input.size()) {
        balls_.push_back(balls);
        return;
      }

      if (input[pos] == ';') {
        balls_.push_back(balls);
        balls = Balls{};
      }
      else {
        assert(input[pos] == ',');
      }

      pos += 2;
      input = input.substr(pos);
    }
  }

  UInt id_{0};
  std::vector<Balls> balls_;
};

auto game_possible(Game const& game, Balls const& max_game) -> bool
{
  return std::ranges::all_of(game.balls_.begin(), game.balls_.end(),
                             [max_game](Balls const& balls) {
                               return balls.red_ <= max_game.red_ &&
                                      balls.green_ <= max_game.green_ &&
                                      balls.blue_ <= max_game.blue_;
                             });
}

auto main() -> int try {
  Balls const max_game{.red_ = 12, .green_ = 13, .blue_ = 14};
  UInt sum{0};

  std::string line;
  while (std::getline(std::cin, line)) {
    Game const game{line};
    if (game_possible(game, max_game)) {
      sum += game.id_;
    }
  }

  std::cout << "Sum of IDs: " << sum << "\n";
  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Unexpected exception\n";
  return EXIT_FAILURE;
}
