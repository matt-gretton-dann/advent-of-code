#include <iostream>
#include <string>
#include <vector>

using UInt = unsigned long;

struct Item
{
  UInt value_;
  unsigned depth_;
};
using Items = std::vector<Item>;

auto print1(Items::const_iterator begin, Items::const_iterator end, unsigned current_depth)
  -> Items::const_iterator
{
  if (begin == end) {
    return begin;
  }

  if (begin->depth_ == current_depth) {
    std::cout << begin->value_;
    ++begin;
    return begin;
  }

  std::cout << '[';
  begin = print1(begin, end, current_depth + 1);
  std::cout << ',';
  begin = print1(begin, end, current_depth + 1);
  std::cout << ']';
  return begin;
}

void print(std::vector<Item> const& items)
{
  print1(items.begin(), items.end(), 0);
  std::cout << '\n';
}

void reduce(std::vector<Item>& items)
{
  /* We do all the explosions followed by all the splits until done.  */
  std::size_t size{0};
  while (size != items.size()) {
    auto it{items.begin()};
    /* Iterate over everything doing as many explosions as we can.  */
    while (it != items.end()) {
      /* Explode */
      if (it->depth_ > 4) {
        assert(it->depth_ == 5);
        assert((it + 1)->depth_ == 5);
        if (it != items.begin()) {
          (it - 1)->value_ += it->value_;
        }
        if (it + 2 != items.end()) {
          (it + 2)->value_ += (it + 1)->value_;
        }
        it->value_ = 0;
        it->depth_ -= 1;
        ++it;
        it = items.erase(it);
      }
      else {
        ++it;
      }
    }

    /* Note the size of items here - we only need to go around again if splitting increases the
     * size.  */
    size = items.size();

    /* Ok we've had no explosions so lets try to do a split.  */
    it = items.begin();
    while (it != items.end()) {
      if (it->value_ > 9) {
        auto value{it->value_};
        auto depth{it->depth_ + 1};
        it = items.insert(it, {value / 2, depth});
        value -= it->value_;
        ++it;
        it->value_ = value;
        it->depth_ = depth;
        /* For this one we need to go back round the loop completely.  */
        it = items.end();
      }
      else {
        ++it;
      }
    }
  }
}

void append(Items& items, std::string const& str)
{
  unsigned current_depth{0};
  for (auto c : str) {
    switch (c) {
    case '[':
      ++current_depth;
      break;
    case ']':
      assert(current_depth != 0);
      --current_depth;
      break;
    case ',':
      continue;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      items.push_back({static_cast<UInt>(c - '0'), current_depth});
      break;
    default:
      std::cerr << "Can't interpret " << c << '\n';
      std::exit(1);
    }
  }
}

auto magnitude1(Items::const_iterator begin, Items::const_iterator end, unsigned current_depth)
  -> std::pair<Items::const_iterator, UInt>
{
  if (begin == end) {
    return {end, 0};
  }

  if (begin->depth_ == current_depth) {
    auto value = begin->value_;
    ++begin;
    return {begin, value};
  }

  auto [lhs_it, lhs] = magnitude1(begin, end, current_depth + 1);
  auto [rhs_it, rhs] = magnitude1(lhs_it, end, current_depth + 1);
  return {rhs_it, 3 * lhs + 2 * rhs};
}

auto magnitude(Items const& items) -> UInt
{
  return magnitude1(items.begin(), items.end(), 0).second;
}

auto add(Items const& lhs, Items const& rhs)
{
  Items result;
  auto inserter{std::back_inserter(result)};
  std::transform(lhs.begin(), lhs.end(), inserter, [](Item const& item) {
    return Item{item.value_, item.depth_ + 1};
  });
  std::transform(rhs.begin(), rhs.end(), inserter, [](Item const& item) {
    return Item{item.value_, item.depth_ + 1};
  });
  reduce(result);
  return result;
}

auto main() -> int
{
  std::string line;
  std::vector<Items> item_list;
  while (std::getline(std::cin, line) && !line.empty()) {
    Items items;
    append(items, line);
    item_list.push_back(items);
  }

  UInt max_magnitude{0};
  for (auto const& lhs : item_list) {
    for (auto const& rhs : item_list) {
      auto result{add(lhs, rhs)};
      auto mag{magnitude(result)};
      if (mag > max_magnitude) {
        max_magnitude = mag;
      }
    }
  }

  std::cout << "Max Magnitude: " << max_magnitude << '\n';
}
