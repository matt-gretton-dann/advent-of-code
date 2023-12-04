#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
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

    [[nodiscard]] auto score() const -> UInt {
        NumVec intersection;
        std::set_intersection(winners_.begin(), winners_.end(), got_.begin(), got_.end(),
                              std::back_inserter(intersection));
        return intersection.empty() ? 0 : (1 << (intersection.size() - 1));
    }

    UInt id_{0};
    NumVec got_;
    NumVec winners_;
};

auto main() -> int try {
    std::string line;
    UInt total_score{0};
    while (std::getline(std::cin, line)) {
        Card card(line);
        total_score += card.score();
    }

    std::cout << "Score: " << total_score << '\n';
    return EXIT_SUCCESS;
}
catch (...) {
    std::cerr << "Uncaught exception.\n";
    return EXIT_FAILURE;
}
