//
// Created by Matthew Gretton-Dann on 07/12/2021.
//

#include <iostream>
#include <list>
#include <string>

/* Consider the case of 8 elves:
 *  Start: 1 2 3 4 5 6 7 8
 *  1 -> 5: 1 2 3 4 6 7 8
 *  2 -> 6: 1 2 3 4 7 8
 *  3 -> 8: 1 2 3 4 7
 *  4 -> 1: 2 3 4 7
 *  7 -> 3: 2 4 7
 *  2 -> 4: 2 7
 *  7 -> 2: 7
 *
 *  Looking at this we can see that we start by removing the element half-way round the list.  The
 *  next element removed is always either the one after the one just removed, or the one just beyond
 *  that.
 *
 *  Which one is removed depends on whether there are an even or odd number of elements left.
 *
 *  We make use of this to realize we don't have to track the current elf who is getting parcels
 *  only the elf that is having parcels stolen from them.
 */
auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read input.\n";
    return 1;
  }
  auto n{std::stoul(line)};
  std::list<unsigned long> elves;
  for (unsigned long i{1}; i <= n; ++i) {
    elves.push_back(i);
  }
  /* it points to the first elf to remove.  */
  auto it{elves.begin()};
  for (unsigned long i{0}; i < n / 2; ++i) {
    ++it;
  }
  while (n > 1) {
    it = elves.erase(it);
    if (it == elves.end()) {
      it = elves.begin();
    }
    if ((--n) % 2 == 0) {
      ++it;
    }
    if (it == elves.end()) {
      it = elves.begin();
    }
  }

  std::cout << "Last elf: " << elves.front() << '\n';
}