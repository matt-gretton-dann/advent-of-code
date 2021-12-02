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

int main(int argc, char** argv)
{
  Time arrival;
  Time closest = ULONG_MAX;
  Time closest_id = 0;
  std::string line;
  std::getline(std::cin, line);
  arrival = std::stoul(line);
  std::cout << "Arrival time: " << arrival << "\n";

  std::string buses;
  std::getline(std::cin, buses);
  std::string::size_type pos = 0;

  while (pos < buses.size()) {
    if (buses[pos] == ',' || buses[pos] == 'x') {
      ++pos;
      continue;
    }

    std::string::size_type len = 0;
    Time depart = std::stoul(buses.substr(pos), &len);
    pos += len;
    auto next_departure_in = depart - (arrival % depart);
    if (next_departure_in == depart) {
      next_departure_in = 0;
    }
    std::cout << "Bus #" << depart << " previous departure: " << (arrival / depart) * depart
              << ", next departure in: " << next_departure_in << "\n";
    if (next_departure_in < closest) {
      closest = next_departure_in;
      closest_id = depart;
    }
  }

  std::cout << "Next departure # " << closest_id << " in " << closest << " minutes.\n";
  std::cout << "Result = " << closest * closest_id << "\n";
  return 0;
}