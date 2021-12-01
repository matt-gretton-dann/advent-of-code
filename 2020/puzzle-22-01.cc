#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

using Card = unsigned;
using Cards = std::list<Card>;
using Score = unsigned long;

struct Player {
  Player(std::istream &is) {
    std::string line;
    if (!std::getline(is, name_)) {
      assert(false);
    }
    while (std::getline(is, line)) {
      if (line == "") {
        break;
      }
      cards_.push_back(std::stoul(line));
    }
  }

  bool empty() const { return cards_.empty(); }
  Card front() const { return cards_.front(); }
  void pop_front() { cards_.pop_front(); }
  void push_back(Card c) { cards_.push_back(c); }

  Score score() const {
    Score r = 0;
    unsigned idx = 1;
    for (auto it = cards_.rbegin(); it != cards_.rend(); ++it) {
      r += idx * *it;
      ++idx;
    }
    return r;
  }

private:
  std::string name_;
  Cards cards_;
};

Score play_game(Player &p1, Player &p2) {
  while (!p1.empty() && !p2.empty()) {
    Card c1 = p1.front();
    Card c2 = p2.front();
    p1.pop_front();
    p2.pop_front();
    if (c1 > c2) {
      p1.push_back(c1);
      p1.push_back(c2);
    } else if (c1 < c2) {
      p2.push_back(c2);
      p2.push_back(c1);
    }
  }

  return p1.score() + p2.score();
}

int main(void) {
  Player player1(std::cin);
  Player player2(std::cin);

  Score final_score = play_game(player1, player2);
  std::cout << "Final score: " << final_score << "\n";
  return 0;
}