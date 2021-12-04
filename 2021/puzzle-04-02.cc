//
// Created by Matthew Gretton-Dann on 04/12/2021.
//

#include <array>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

using Number = unsigned long;  // NOLINT(google-runtime-int)
using NumberVector = std::vector<Number>;

auto parse_number_list(std::string const& s) -> NumberVector
{
  std::size_t idx{0};
  NumberVector result;
  while (idx < s.size()) {
    std::size_t end{0};
    result.push_back(std::stoul(s.substr(idx), &end));
    idx += end;
    while (idx < s.size() && s[idx] == ',') {
      ++idx;
    }
  }
  return result;
}

struct BingoCard
{
public:
  explicit BingoCard(std::array<std::string, 5> const& card)
  {
    unsigned row{0};
    for (auto const& s : card) {
      std::size_t idx{0};
      for (unsigned col{0}; col < 5; ++col) {
        while (s.at(idx) == ' ') {
          ++idx;
        }
        std::size_t end{0};
        numbers_.at(row + col) = {std::stoul(s.substr(idx), &end), false};
        idx += end;
      }
      assert(idx == s.size());

      row += 5;
    }
  }

  void mark(unsigned num)
  {
    for (auto it{// NOLINT(llvm-qualified-auto,readability-qualified-auto)
                 find_num(numbers_.begin(), numbers_.end(), num)};
         it != numbers_.end(); it = find_num(++it, numbers_.end(), num)) {
      it->second = true;
    }
  }

  [[nodiscard]] auto bingo() const -> bool
  {
    for (unsigned row{0}; row < 25; row += 5) {
      if (std::all_of(numbers_.begin() + row, numbers_.begin() + row + 5,
                      [](auto const& n) { return n.second; })) {
        return true;
      }
    }

    for (unsigned col{0}; col < 5; ++col) {
      unsigned marked_count{0};
      for (unsigned row{col}; row < 25; row += 5) {
        if (numbers_.at(row).second) {
          ++marked_count;
        }
      }
      if (marked_count == 5) {
        return true;
      }
    }

    return false;
  }

  [[nodiscard]] auto score() const -> Number
  {
    return std::accumulate(numbers_.begin(), numbers_.end(), Number{0},
                           [](Number a, auto const& n) { return a + (n.second ? 0 : n.first); });
  }

  auto print(std::ostream& os) const -> std::ostream&
  {
    for (unsigned i = 0; i < 25; ++i) {
      os << std::setw(3) << numbers_.at(i).first << (numbers_.at(i).second ? "!" : " ")
         << ((i % 5 == 4) ? '\n' : ' ');
    }
    return os;
  }

private:
  using NumberPair = std::pair<Number, bool>;
  using NumberArray = std::array<NumberPair, 25>;

  static auto find_num(NumberArray::iterator begin, NumberArray::iterator end, Number num)
    -> NumberArray ::iterator
  {
    return std::find_if(begin, end, [num](auto const& n) { return n.first == num; });
  }

  NumberArray numbers_;
};

auto main() -> int
{
  std::string line;

  if (!getline(std::cin, line)) {
    std::cerr << "Unable to read random numbers.\n";
    return 1;
  }
  NumberVector random_numbers{parse_number_list(line)};

  std::vector<BingoCard> cards;
  while (std::getline(std::cin, line)) {
    assert(line.empty());
    std::array<std::string, 5> lines;
    for (unsigned row{0}; row < 5; ++row) {
      std::getline(std::cin, lines.at(row));
    }
    cards.emplace_back(lines);
  }

  auto num{random_numbers.begin()};
  while (cards.size() > 1 && num != random_numbers.end()) {
    for (auto card{cards.begin()}; card != cards.end();
         card = (card->bingo() ? cards.erase(card) : ++card)) {
      card->mark(*num);
    }
    ++num;
  }

  assert(cards.size() == 1);

  while (!cards[0].bingo() && num != random_numbers.end()) {
    cards[0].mark(*num++);
  }
  --num;

  auto score{cards[0].score()};
  std::cout << "\nBINGO " << score << " * " << *num << " = " << score * *num << '\n';
  cards[0].print(std::cout);
  return 0;
}