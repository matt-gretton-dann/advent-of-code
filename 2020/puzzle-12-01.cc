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
  Position() : x_(0), y_(0), head_(Heading::East) {}

  void move(std::string const &s) {
    char act = s[0];
    int dist = std::stoi(s.substr(1));

    switch (act) {
    case 'N':
    case 'E':
    case 'S':
    case 'W':
      move(dist, static_cast<Heading>(act));
      break;
    case 'F':
      move(dist, head_);
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

    std::cout << s << ": (" << x_ << ", " << y_ << ")\n";
  }

  void move(int dist, Heading head) {
    switch (head) {
    case Heading::North:
      y_ += dist;
      break;
    case Heading::East:
      x_ += dist;
      break;
    case Heading::South:
      y_ -= dist;
      break;
    case Heading::West:
      x_ -= dist;
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
      switch (head_) {
      case Heading::North:
        head_ = Heading::East;
        break;
      case Heading::East:
        head_ = Heading::South;
        break;
      case Heading::South:
        head_ = Heading::West;
        break;
      case Heading::West:
        head_ = Heading::North;
        break;
      default:
        assert(false);
      }
    } else if (deg == 90) {
      switch (head_) {
      case Heading::North:
        head_ = Heading::West;
        break;
      case Heading::East:
        head_ = Heading::North;
        break;
      case Heading::South:
        head_ = Heading::East;
        break;
      case Heading::West:
        head_ = Heading::South;
        break;
      default:
        assert(false);
      }
    } else if (deg == 180 || deg == -180) {
      switch (head_) {
      case Heading::North:
        head_ = Heading::South;
        break;
      case Heading::East:
        head_ = Heading::West;
        break;
      case Heading::South:
        head_ = Heading::North;
        break;
      case Heading::West:
        head_ = Heading::East;
        break;
      default:
        assert(false);
      }
    } else if (deg != 0) {
      assert(false);
    }
  }

  int distance() const noexcept { return std::abs(x_) + std::abs(y_); }

  int x_;
  int y_;
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