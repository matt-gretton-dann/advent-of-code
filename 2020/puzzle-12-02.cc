#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

enum class Heading { North = 'N', East = 'E', South = 'S', West = 'W' };

struct Position {
  Position() : x_(0), y_(0), wx_(10), wy_(1), head_(Heading::East) {}

  void move(std::string const &s) {
    char act = s[0];
    int dist = std::stoi(s.substr(1));

    switch (act) {
    case 'N':
    case 'E':
    case 'S':
    case 'W':
      move_way(dist, static_cast<Heading>(act));
      break;
    case 'F':
      x_ += wx_ * dist;
      y_ += wy_ * dist;
      break;
    case 'L':
      rotate_left(dist);
      break;
    case 'R':
      rotate_left(360 - dist);
      break;
    default:
      assert(false);
    }

    std::cout << s << ": Pos = (" << x_ << ", " << y_ << "), Way = (" << wx_
              << ", " << wy_ << ")\n";
  }

  void move_way(int dist, Heading head) {
    switch (head) {
    case Heading::North:
      wy_ += dist;
      break;
    case Heading::East:
      wx_ += dist;
      break;
    case Heading::South:
      wy_ -= dist;
      break;
    case Heading::West:
      wx_ -= dist;
      break;
    default:
      assert(false);
      break;
    }
  }

  void rotate_left(int deg) {
    /* We want the rang [-180, 180]. */
    while (deg < -180) {
      deg += 360;
    }
    while (deg > 180) {
      deg -= 360;
    }

    if (deg == -90) {
      int px = wx_;
      int py = wy_;
      wx_ = py;
      wy_ = -px;
    } else if (deg == 90) {
      int px = wx_;
      int py = wy_;
      wx_ = -py;
      wy_ = px;
    } else if (deg == 180 || deg == -180) {
      wx_ = -wx_;
      wy_ = -wy_;
    } else if (deg != 0) {
      assert(false);
    }
  }

  int distance() const noexcept { return std::abs(x_) + std::abs(y_); }

  int x_;
  int y_;
  int wx_;
  int wy_;
  Heading head_;
};

int main(int argc, char **argv) {
  Position pos;

  for (std::string line; std::getline(std::cin, line);) {
    pos.move(line);
  }

  auto dist = pos.distance();
  std::cout << "Distance: " << dist << "\n";
  return 0;
}