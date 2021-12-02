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
using Hash = std::string;

struct Player
{
  Player(std::istream& is)
  {
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
  std::size_t size() const { return cards_.size(); }

  Score score() const
  {
    Score r = 0;
    unsigned idx = 1;
    for (auto it = cards_.rbegin(); it != cards_.rend(); ++it) {
      r += idx * *it;
      ++idx;
    }
    return r;
  }

  Hash hash() const
  {
    std::string r = name_;
    for (auto c : cards_) {
      assert(c > 0);
      assert(c < 128);
      r += (char)c;
    }

    return r;
  }

  Player(Player const& prev, Card size) : name_(prev.name_ + "-")
  {
    auto it = prev.cards_.begin();
    for (Card i = 0; i < size; ++i) {
      cards_.push_back(*it++);
    }
  }

  friend std::ostream& operator<<(std::ostream& os, Player const& p);

private:
  std::string name_;
  Cards cards_;
};

std::ostream& operator<<(std::ostream& os, Player const& p)
{
  os << p.name_;
  for (auto c : p.cards_) {
    os << " " << c;
  }
  return os;
}

// False = p1 wins, true = p2 wins.
bool play_game(Player& p1, Player& p2)
{
  std::set<Hash> hashes;

  while (!p1.empty() && !p2.empty()) {
    // Ensure we've not seen this configuration before.
    auto [it, success] = hashes.insert(p1.hash() + p2.hash());
    if (!success) {
      return false;
    }

    Card c1 = p1.front();
    Card c2 = p2.front();
    p1.pop_front();
    p2.pop_front();
    if (p1.size() >= c1 && p2.size() >= c2) {
      Player sp1(p1, c1);
      Player sp2(p2, c2);
      bool result = play_game(sp1, sp2);
      if (!result) {
        p1.push_back(c1);
        p1.push_back(c2);
      }
      else {
        p2.push_back(c2);
        p2.push_back(c1);
      }
    }
    else if (c1 > c2) {
      p1.push_back(c1);
      p1.push_back(c2);
    }
    else if (c1 < c2) {
      p2.push_back(c2);
      p2.push_back(c1);
    }
    else {
      assert(false);
    }
  }

  return p1.empty();
}

int main(void)
{
  Player player1(std::cin);
  Player player2(std::cin);

  play_game(player1, player2);
  Score final_score = player1.score() + player2.score();
  std::cout << "Final score: " << final_score << "\n";
  return 0;
}