#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

using Time = unsigned long;
using IDPair = std::pair<Time, Time>;
using IDVector = std::vector<IDPair>;

int main(int argc, char **argv) {
  std::string line;
  std::getline(std::cin, line); // Ignore first line

  std::string buses;
  std::getline(std::cin, buses);
  std::string::size_type pos = 0;
  Time offset = 0;
  IDVector ids;

  while (pos < buses.size()) {
    if (buses[pos] == ',') {
      ++pos;
      continue;
    } else if (buses[pos] == 'x') {
      ++offset;
      ++pos;
      continue;
    }

    std::string::size_type len = 0;
    Time depart = std::stoul(buses.substr(pos), &len);
    std::cout << "Bus #" << depart << " at offset " << offset << "\n";
    ids.push_back(std::make_pair(depart, offset));
    pos += len;
    ++offset;
  }

  // Make assumption that all bus IDs are co-prime.
  Time t = 0;
  Time t_incr = 1;
  for (auto const &bus : ids) {
    while ((t + bus.second) % bus.first != 0) {
      t += t_incr;
    }
    std::cout << "Bus #" << bus.first << " at offset " << bus.second
              << " t = " << t << "\n";
    t_incr *= bus.first;
  }
  return 0;
}