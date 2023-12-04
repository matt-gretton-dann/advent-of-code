#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <string>
#include <vector>

using UInt = std::uint64_t;
using NumVec = std::vector<UInt>;

struct Card {
    explicit Card(std::string line) {
      assert(line.substr(0, 4) == "Card");
      line = line.substr(4);

      /* Get the ID. */
      std::size_t pos{0};
      id_ = std::stoull(line, &pos);
      assert(line[pos] == ':');
      line = line.substr(pos + 1);

      /* Read in the winners. */
      while (line[0] != '|') {
        winners_.push_back(std::stoull(line, &pos));
        while (line[pos] == ' ') { ++pos; }
        line = line.substr(pos);
      }
      std::sort(winners_.begin(), winners_.end());

      /* Read in the numbers we have. */
      line = line.substr(1);
      while (!line.empty()) {
        got_.push_back(std::stoull(line, &pos));
        line = line.substr(pos);
      }
      std::sort(got_.begin(), got_.end());
    }

    [[nodiscard]] auto operator<(Card const &rhs) const -> bool {
      return id_ < rhs.id_;
    }

    [[nodiscard]] auto match_count() const -> UInt {
      NumVec intersection;
      std::set_intersection(winners_.begin(), winners_.end(), got_.begin(), got_.end(),
                            std::back_inserter(intersection));
      return intersection.size();
    }

    UInt id_{0};
    NumVec got_;
    NumVec winners_;
};

using CardMap = std::map<Card, UInt>;

auto main() -> int try {
  std::string line;
  CardMap cards;
  while (std::getline(std::cin, line)) {
    Card card(line);
    cards.insert({card, 1});
  }

  for (auto it{cards.begin()}; it != cards.end(); ++it) {
    auto matched = it->first.match_count();
    auto it2 = it;
    while (matched-- != 0) {
      ++it2;
      it2->second += it->second;
    }
  }

  auto count = std::accumulate(cards.begin(), cards.end(), UInt{0},
                               [](UInt a, auto const &it) { return a + it.second; });
  std::cout << "Final count of cards: " << count << '\n';
  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
