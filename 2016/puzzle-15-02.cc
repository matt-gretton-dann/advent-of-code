//
// Created by Matthew Gretton-Dann on 06/12/2021.
//

#include <iostream>
#include <regex>
#include <string>
#include <vector>

struct Circle
{
  explicit Circle(std::string const& s)
  {
    std::regex re{"Disc #\\d has (\\d+) positions; at time=0, it is at position (\\d+)."};
    std::smatch m;
    if (!std::regex_search(s, m, re)) {
      abort();
    }
    circumference_ = std::stoul(m.str(1));
    start_ = std::stoul(m.str(2));
  }

  Circle(unsigned circumference, unsigned start) : circumference_(circumference), start_(start) {}

  auto circumference() const noexcept -> unsigned { return circumference_; }
  auto start() const noexcept -> unsigned { return start_; }

private:
  unsigned circumference_;
  unsigned start_;
};

auto main() -> int
{
  std::string line;
  std::vector<Circle> circles;
  while (std::getline(std::cin, line)) {
    circles.emplace_back(line);
  }
  circles.emplace_back(11, 0);

  unsigned time{0};
  unsigned adder{1};
  unsigned offset{0};
  /* For each circle we want the value of cirle.start() + offset + time to be 0 (mod number of
   * positions).  Where the offset is how many circles we've dropped through (starting at 1), and
   * time is the time released.
   *
   * Once we've worked out the time for the first circle, all future
   * potential releases will be (t1 + N * pos1), for circle 2 it will be (t2 + N * pos1 * pos2).
   * We use adder to keep track of this.
   *
   * We assume all circumferences are co-prime.
   */
  for (auto const& circle : circles) {
    ++offset;
    while ((circle.start() + offset + time) % circle.circumference() != 0) {
      time += adder;
    }
    std::cout << "Circle start " << circle.start() << " Position " << circle.circumference()
              << " release time " << time << '\n';
    adder *= circle.circumference();
  }

  std::cout << "Release time: " << time << '\n';
  return 0;
}