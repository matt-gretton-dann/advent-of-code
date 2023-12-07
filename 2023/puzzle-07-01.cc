#include <algorithm>
#include <array>
#include <cassert>
#include <compare>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

using UInt = std::uint64_t;
using Cards = std::array<char, 5>;

enum class HandType
{
  high_card,
  one_pair,
  two_pair,
  three,
  full_house,
  four,
  five
};

auto operator<<(std::ostream& os, HandType type) -> std::ostream&
{
  switch (type) {
  case HandType::high_card:
    return os << "High card";
  case HandType::one_pair:
    return os << "One pair";
  case HandType::two_pair:
    return os << "Two pair";
  case HandType::three:
    return os << "Three of a kind";
  case HandType::full_house:
    return os << "Full house";
  case HandType::four:
    return os << "Four of a kind";
  case HandType::five:
    return os << "Five of a kind";
  }
}

[[nodiscard]] auto operator<=>(HandType lhs, HandType rhs) -> std::strong_ordering
{
  auto const l{static_cast<std::underlying_type_t<HandType>>(lhs)};
  auto const r{static_cast<std::underlying_type_t<HandType>>(rhs)};
  return l <=> r;
}

auto operator<<(std::ostream& os, Cards const& cards) -> std::ostream&
{
  for (auto const c : cards) { os << c; }
  return os;
}

[[nodiscard]] auto operator<=>(Cards const& lhs, Cards const& rhs) -> std::strong_ordering
{
  static auto const* order = "23456789TJQKA";
  for (auto lhs_it{lhs.begin()}, rhs_it{rhs.begin()}; lhs_it != lhs.end(); ++lhs_it, ++rhs_it) {
    if (auto l = std::strchr(order, *lhs_it), r = std::strchr(order, *rhs_it); l != r) {
      return l <=> r;
    }
  }
  return std::strong_ordering::equal;
}

[[nodiscard]] auto classify(Cards const& cards) -> HandType
{
  std::map<char, unsigned> holding;
  for (auto card : cards) { holding.insert({card, 0}).first->second++; }
  unsigned max_count{1};
  unsigned pairs_seen{0};
  for (auto [card, count] : holding) {
    max_count = std::max(count, max_count);
    if (count == 2) { ++pairs_seen; }
  }
  if (max_count == 5) { return HandType::five; }
  if (max_count == 4) { return HandType::four; }
  if (max_count == 3 && pairs_seen == 1) { return HandType::full_house; }
  if (max_count == 3) { return HandType::three; }
  if (max_count == 2 && pairs_seen == 2) { return HandType::two_pair; }
  if (max_count == 2 && pairs_seen == 1) { return HandType::one_pair; }
  assert(max_count == 1);
  return HandType::high_card;
}

struct Hand
{
  explicit Hand(std::string_view line)
    : cards_({line[0], line[1], line[2], line[3], line[4]}), type_(classify(cards_)),
      bid_(std::strtoul(line.data() + 5, nullptr, 10))
  {
  }

  [[nodiscard]] auto operator<=>(Hand const& rhs) const -> std::strong_ordering
  {
    auto type_order = type_ <=> rhs.type_;
    if (type_order != std::strong_ordering::equal) { return type_order; }
    return cards_ <=> rhs.cards_;
  }

  Cards cards_;
  HandType type_;
  UInt bid_;
};

auto operator<<(std::ostream& os, Hand const& hand) -> std::ostream&
{
  return os << std::setw(16) << hand.type_ << ' ' << hand.cards_ << ' ' << std::setw(5) << hand.
         bid_;
}


auto main() -> int try {
  std::string line;
  std::vector<Hand> hands;

  while (std::getline(std::cin, line)) {
    hands.emplace_back(line);
  }

  std::sort(hands.begin(), hands.end());

  UInt score{0};
  for (std::size_t i{0}; i != hands.size(); ++i) {
    score += (i + 1) * hands[i].bid_;
    std::cout << std::setw(5) << i + 1 << ' ' << hands[i] << ' ' << score << '\n';
  }

  std::cout << "Score: " << score;

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
