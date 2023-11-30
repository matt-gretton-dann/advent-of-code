#include <cassert>
#include <array>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <vector>

using UInt = std::uint64_t;

/* We're going to maintain state in a [0, 21] x [0, 21] grid.  Where the position in the grid
 * represents (player 1 score, player 2 score).  We start in position (0,0).  We then do player 1's
 * turn removing them from position (0,0) and updating all possible scores, along with the positions
 * they can be in.  Then from those states we play player 2's turn.  If we end up at a score of >=
 * 21 we just put the result in the [21,y] or [x,21] value as appropriate.
 *
 * Then when there's nothing to do in [0,20] x [0,20] we just have to sum up the last column and
 * last row to get the number of victories.
 *
 * So each grid position needs to maintain number of states with that score for each position on the
 * track.
 */

struct State
{
  constexpr static unsigned target_score{21};
  State(unsigned initial_posx, unsigned initial_posy) { get(0, 0, initial_posx, initial_posy) = 1; }

  [[nodiscard]] auto in_play() const noexcept -> bool
  {
    return std::any_of(data_.begin(), data_.end(), [](UInt x) { return x > 0; });
  }

  [[nodiscard]] auto size1() const noexcept -> UInt { return player1_wins_; }
  [[nodiscard]] auto size2() const noexcept -> UInt { return player2_wins_; }

  [[nodiscard]] auto move() const noexcept -> State
  {
    State result1;
    result1.player1_wins_ = player1_wins_;
    result1.player2_wins_ = player2_wins_;

    for (unsigned scorex{0}; scorex < target_score; ++scorex) {
      for (unsigned scorey{0}; scorey < target_score; ++scorey) {
        for (unsigned posx{1}; posx <= loop_size; ++posx) {
          for (unsigned posy{1}; posy <= loop_size; ++posy) {
            auto states{get(scorex, scorey, posx, posy)};
            if (states == 0) {
              continue;
            }
            for (unsigned die1{1}; die1 < 4; ++die1) {
              for (unsigned die2{1}; die2 < 4; ++die2) {
                for (unsigned die3{1}; die3 < 4; ++die3) {
                  result1.move_player1(scorex, scorey, posx, posy, die1 + die2 + die3, states);
                }
              }
            }
          }
        }
      }
    }
    result1.print();

    State result2;
    result2.player1_wins_ = result1.player1_wins_;
    result2.player2_wins_ = result1.player2_wins_;

    for (unsigned scorex{0}; scorex < target_score; ++scorex) {
      for (unsigned scorey{0}; scorey < target_score; ++scorey) {
        for (unsigned posx{1}; posx <= loop_size; ++posx) {
          for (unsigned posy{1}; posy <= loop_size; ++posy) {
            auto states{result1.get(scorex, scorey, posx, posy)};
            if (states == 0) {
              continue;
            }
            for (unsigned die1{1}; die1 < 4; ++die1) {
              for (unsigned die2{1}; die2 < 4; ++die2) {
                for (unsigned die3{1}; die3 < 4; ++die3) {
                  result2.move_player2(scorex, scorey, posx, posy, die1 + die2 + die3, states);
                }
              }
            }
          }
        }
      }
    }

    result2.print();

    return result2;
  }

  void print() const
  {
    std::cout << std::string(100, '-') << '\n';
    for (unsigned scorex{0}; scorex < target_score; ++scorex) {
      for (unsigned scorey{0}; scorey < target_score; ++scorey) {
        UInt count{0};
        for (unsigned posx{1}; posx <= loop_size; ++posx) {
          for (unsigned posy{1}; posy <= loop_size; ++posy) {
            count += get(scorex, scorey, posx, posy);
          }
        }
        std::cout << std::setw(6) << count << ' ';
      }
      std::cout << '\n';
    }
    std::cout << "Player 1 wins: " << player1_wins_ << '\n';
    std::cout << "Player 2 wins: " << player2_wins_ << '\n';
  }

  void move_player1(unsigned scorex, unsigned scorey, unsigned posx, unsigned posy, unsigned amount,
                    UInt states)
  {
    auto new_posx{posx + amount % loop_size};
    if (new_posx > loop_size) {
      new_posx -= loop_size;
    }
    auto new_scorex{scorex + new_posx};
    if (new_scorex >= target_score) {
      player1_wins_ += states;
    }
    else {
      get(new_scorex, scorey, new_posx, posy) += states;
    }
  }

  void move_player2(unsigned scorex, unsigned scorey, unsigned posx, unsigned posy, unsigned amount,
                    UInt states)
  {
    auto new_posy{posy + amount % loop_size};
    if (new_posy > loop_size) {
      new_posy -= loop_size;
    }
    auto new_scorey{scorey + new_posy};
    if (new_scorey >= target_score) {
      player2_wins_ += states;
    }
    else {
      get(scorex, new_scorey, posx, new_posy) += states;
    }
  }

private:
  State() noexcept = default;

  auto get(unsigned scorex, unsigned scorey, unsigned posx, unsigned posy) const noexcept -> UInt
  {
    return data_[scorey * target_score * loop_size * loop_size + scorex * loop_size * loop_size +
                 (posy - 1) * loop_size + (posx - 1)];
  }

  auto get(unsigned scorex, unsigned scorey, unsigned posx, unsigned posy) noexcept -> UInt&
  {
    return data_[scorey * target_score * loop_size * loop_size + scorex * loop_size * loop_size +
                 (posy - 1) * loop_size + (posx - 1)];
  }

  constexpr static unsigned loop_size{10};

  std::array<UInt, (loop_size * loop_size * target_score * target_score)> data_{0};
  UInt player1_wins_{0};
  UInt player2_wins_{0};
};

auto main() -> int
{
  std::string line;
  std::regex re{"Player (\\d) starting position: (\\d+)"};

  std::vector<unsigned> players;
  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Unable to match: " << line << "\n";
      return 1;
    }
    players.push_back(std::stol(m.str(2)));
  }
  assert(players.size() == 2);
  State state(players[0], players[1]);

  while (state.in_play()) {
    state = state.move();
  }

  auto player1_wins{state.size1()};
  auto player2_wins{state.size2()};
  std::cout << "Player 1 wins: " << player1_wins << '\n';
  std::cout << "Player 2 wins: " << player2_wins << '\n';
  std::cout << "Max: " << std::max(player1_wins, player2_wins);
  return 0;
}
