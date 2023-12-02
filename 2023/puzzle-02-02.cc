#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numeric>
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

  [[nodiscard]] auto power() const -> UInt
  {
    Balls const balls = std::accumulate(balls_.begin(), balls_.end(),
                                        Balls{}, [](Balls acc, Balls const& b) {
                                          if (b.red_ > acc.red_) { acc.red_ = b.red_; }
                                          if (b.green_ > acc.green_) { acc.green_ = b.green_; }
                                          if (b.blue_ > acc.blue_) { acc.blue_ = b.blue_; }
                                          return acc;
                                        });
    return balls.red_ * balls.green_ * balls.blue_;
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
  UInt sum1{0};
  UInt power_sum{0};

  std::string line;
  while (std::getline(std::cin, line)) {
    Game const game{line};
    if (game_possible(game, max_game)) {
      sum1 += game.id_;
    }
    power_sum += game.power();
  }

  std::cout << "Sum of IDs: " << sum1 << "\n";
  std::cout << "Sum of power: " << power_sum << "\n";
  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Unexpected exception\n";
  return EXIT_FAILURE;
}
