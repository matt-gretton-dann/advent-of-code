#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <vector>

using UInt = unsigned;

struct PlayerState
{
  constexpr PlayerState(UInt initial_pos) : pos_(initial_pos) {}

  [[nodiscard]] constexpr auto score() const noexcept -> UInt { return score_; }
  [[nodiscard]] constexpr auto pos() const noexcept -> UInt { return pos_; }

  constexpr void move(UInt amount) noexcept
  {
    amount %= 10;
    pos_ += amount;
    if (pos_ > 10) {
      pos_ -= 10;
    }
    score_ += pos_;
  }

  UInt pos_;
  UInt score_{0};
};

struct Die
{
  [[nodiscard]] constexpr auto roll_count() const noexcept -> UInt { return roll_count_; }

  [[nodiscard]] constexpr auto roll() noexcept -> UInt
  {
    ++roll_count_;
    ++current_;
    if (current_ == 101) {
      current_ = 1;
    }
    return current_;
  }

private:
  UInt current_{0};
  UInt roll_count_{0};
};

auto main() -> int
{
  std::string line;
  std::regex re{"Player (\\d) starting position: (\\d+)"};

  std::vector<PlayerState> players;
  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Unable to match: " << line << "\n";
      return 1;
    }
    players.push_back(PlayerState(std::stol(m.str(2))));
  }

  Die die;
  bool cont{true};
  while (cont) {
    for (auto& player : players) {
      UInt move = die.roll() + die.roll() + die.roll();
      player.move(move);
      if (player.score() >= 1'000) {
        cont = false;
        break;
      }
    }
  }

  auto r{
    std::accumulate(players.begin(), players.end(), UInt{die.roll_count()},
                    [](auto a, auto const& p) { return a * (p.score() < 1000 ? p.score() : 1); })};
  std::cout << "Multiplied scores " << r << '\n';
  return 0;
}
