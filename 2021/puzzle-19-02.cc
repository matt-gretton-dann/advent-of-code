#include <array>
#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using Int = long;
struct Coord
{
  constexpr Coord(Int x, Int y, Int z) noexcept : x_(x), y_(y), z_(z) {}

  explicit Coord(std::string const& s)
  {
    std::size_t idx{0};
    x_ = std::stol(s, &idx);
    assert(idx < s.size() && s[idx] == ',');
    ++idx;
    std::size_t end{0};
    y_ = std::stol(s.substr(idx), &end);
    idx += end;
    assert(idx < s.size() && s[idx] == ',');
    ++idx;
    z_ = std::stol(s.substr(idx));
  }

  [[nodiscard]] auto x() const noexcept -> Int { return x_; }
  [[nodiscard]] auto y() const noexcept -> Int { return y_; }
  [[nodiscard]] auto z() const noexcept -> Int { return z_; }

  auto operator+=(Coord const& c) -> Coord
  {
    x_ += c.x_;
    y_ += c.y_;
    z_ += c.z_;
    return *this;
  }

  auto operator-=(Coord const& c) -> Coord
  {
    x_ -= c.x_;
    y_ -= c.y_;
    z_ -= c.z_;
    return *this;
  }

  auto operator<(Coord const& c) const -> bool
  {
    return x_ < c.x_ || (x_ == c.x_ && y_ < c.y_) || (x_ == c.x_ && y_ < c.y_ && z_ < c.z_);
  }

private:
  Int x_{0};
  Int y_{0};
  Int z_{0};
};

auto operator<<(std::ostream& os, Coord const& c) -> std::ostream&
{
  os << "[" << c.x() << ", " << c.y() << ", " << c.z() << "]";
  return os;
}

auto operator+(Coord const& lhs, Coord const& rhs) -> Coord
{
  auto result(lhs);
  result += rhs;
  return result;
}

auto operator-(Coord const& lhs, Coord const& rhs) -> Coord
{
  auto result(lhs);
  result -= rhs;
  return result;
}

auto operator-(Coord const& lhs) -> Coord { return {-lhs.x(), -lhs.y(), -lhs.z()}; }

auto manhattan_distance(Coord const& lhs, Coord const& rhs) -> Int
{
  return std::abs(lhs.x() - rhs.x()) + std::abs(lhs.y() - rhs.y()) + std::abs(lhs.z() - rhs.z());
}

using CoordVector = std::vector<Coord>;
using CoordSet = std::set<Coord>;
using Transform = std::array<Coord, 3>;

auto operator<<(std::ostream& os, Transform const& t) -> std::ostream&
{
  os << "[[" << t[0].x() << ' ' << t[0].y() << ' ' << t[0].z() << ']';
  os << '[' << t[1].x() << ' ' << t[1].y() << ' ' << t[1].z() << ']';
  os << '[' << t[2].x() << ' ' << t[2].y() << ' ' << t[2].z() << "]]";
  return os;
}

auto transform(Coord const& c, Transform const& t) -> Coord
{
  return {c.x() * t[0].x() + c.y() * t[0].y() + c.z() * t[0].z(),
          c.x() * t[1].x() + c.y() * t[1].y() + c.z() * t[1].z(),
          c.x() * t[2].x() + c.y() * t[2].y() + c.z() * t[2].z()};
}

constexpr Coord x{1, 0, 0};
constexpr Coord y{0, 1, 0};
constexpr Coord z{0, 0, 1};

Transform const transforms[24] = {{-z, -y, -x}, {-z, -x, y}, {-z, x, -y}, {-z, y, x},   {-y, -z, x},
                                  {-y, -x, -z}, {-y, x, z},  {-y, z, -x}, {-x, -z, -y}, {-x, -y, z},
                                  {-x, y, -z},  {-x, z, y},  {x, -z, y},  {x, -y, -z},  {x, y, z},
                                  {x, z, -y},   {y, -z, -x}, {y, -x, z},  {y, x, -z},   {y, z, x},
                                  {z, -y, x},   {z, -x, -y}, {z, x, y},   {z, y, -x}};

struct Scanner
{
  void absolute_position(Coord const& c) { absolute_position_ = c; }
  [[nodiscard]] auto absolute_position() const -> Coord const& { return absolute_position_; }
  [[nodiscard]] auto located() const -> bool { return located_; }
  void locate() { located_ = true; }
  [[nodiscard]] auto scanned() const -> bool { return scanned_; }
  void scan() { scanned_ = true; }

  [[nodiscard]] auto probes_begin() const { return probes_.begin(); }
  [[nodiscard]] auto probes_end() const { return probes_.end(); }
  [[nodiscard]] auto probes_begin() { return probes_.begin(); }
  [[nodiscard]] auto probes_end() { return probes_.end(); }
  [[nodiscard]] auto size() const -> std::size_t { return probes_.size(); }

  void insert_probe(Coord const& c) { probes_.push_back(c); }
  Coord absolute_position_{0, 0, 0};
  bool located_{false};
  bool scanned_{false};
  CoordVector probes_;
};

using Scanners = std::vector<Scanner>;

auto main() -> int
{
  std::string line;
  Scanners scanners;

  while (std::getline(std::cin, line) && !line.empty()) {
    if (line.substr(0, 3) != "---") {
      std::cerr << "Unexpected header.\n";
      return 1;
    }
    scanners.emplace_back();
    while (std::getline(std::cin, line) && !line.empty()) {
      scanners.back().insert_probe(Coord{line});
    }
  }

  /* We locate the first scanner in the default position.  */
  scanners[0].locate();

  while (!std::all_of(scanners.begin(), scanners.end(),
                      [](auto const& scanner) { return scanner.located(); })) {
    /* While we have something to locate.  */
    for (auto it{scanners.begin()}; it != scanners.end(); ++it) {
      /* Outer loop: loops over all located nodes - we have at least one (the first).  */
      if (!it->located() || it->scanned()) {
        continue;
      }
      it->scan();

      for (auto it2{scanners.begin()}; it2 != scanners.end(); ++it2) {
        /* Inner loop: loops over all un-located nodes.  */
        if (it2->located()) {
          continue;
        }

        /* Now iterate over the possible locations for the scanners relatvie to each other.  */
        for (auto base_position{it->probes_begin()};
             base_position != it->probes_end() && !it2->located(); ++base_position) {
          for (auto probe_it{it2->probes_begin()}; probe_it != it2->probes_end() && !it2->located();
               ++probe_it) {
            for (auto const& t : transforms) {
              auto abs_position{*base_position - transform(*probe_it, t)};
              CoordSet probes;
              auto inserter{std::inserter(probes, probes.end())};
              std::copy(it->probes_begin(), it->probes_end(), inserter);
              std::transform(
                it2->probes_begin(), it2->probes_end(), inserter,
                [&abs_position, &t](Coord const& c) { return transform(c, t) + abs_position; });

              if (probes.size() + 12 <= it->size() + it2->size()) {
                /* Move probes to expected position.  */
                it2->absolute_position(abs_position);
                std::for_each(
                  it2->probes_begin(), it2->probes_end(),
                  [&abs_position, &t](Coord& c) { c = transform(c, t) + abs_position; });
                std::cout << "Located: " << it2 - scanners.begin() << '\n';
                it2->locate();
                break;
              }
            }
          }
        }
      }
    }
  }

  CoordSet probes;
  Int max_dist{0};
  for (auto const& scanner : scanners) {
    probes.insert(scanner.probes_begin(), scanner.probes_end());
    for (auto const& scanner2 : scanners) {
      auto dist{manhattan_distance(scanner.absolute_position(), scanner2.absolute_position())};
      max_dist = std::max(dist, max_dist);
    }
  }

  std::cout << "Number of probes: " << probes.size() << '\n';
  std::cout << "Maximum distance: " << max_dist << '\n';
  return 0;
}
