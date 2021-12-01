#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <tuple>

using Id = unsigned long;
using Hash = unsigned long;

enum Edge { Top, Left, Bottom, Right };

struct PictureArray;

struct Picture {
  Picture(std::string id, std::istream &is) : in_use_(false) {
    assert(id.substr(0, 5) == "Tile ");
    id_ = std::stoul(id.substr(5));
    std::string line;
    while (std::getline(is, line)) {
      if (line.empty()) {
        return;
      }
      rows_.push_back(line);
    }
  }

  Picture(PictureArray const &array);

  Picture(Picture const &) = delete;
  Picture &operator=(Picture const &) = delete;
  Picture(Picture &&) = default;
  Picture &operator=(Picture &&) = default;

  void flip() {
    for (auto &r : rows_) {
      std::reverse(r.begin(), r.end());
    }
  }

  void rotate() {
    std::vector<std::string> copy(rows_.size());
    for (auto const &r : rows_) {
      std::size_t off = copy.size();
      assert(r.size() == copy.size());
      for (auto c : r) {
        copy[--off] += c;
      }
      assert(off == 0);
    }
    rows_ = copy;
  }

  Hash hash(Edge edge) const {
    unsigned x = (edge == Edge::Right) ? rows_[0].size() - 1 : 0;
    unsigned y = (edge == Edge::Bottom) ? rows_.size() - 1 : 0;
    unsigned dx = (edge == Edge::Top || edge == Edge::Bottom) ? 1 : 0;
    unsigned dy = (edge == Edge::Left || edge == Edge::Right) ? 1 : 0;

    Hash hash = 0;
    for (unsigned i = 0; i < rows_.size(); ++i) {
      hash <<= 1;
      hash |= (rows_[y][x] == '#');
      y += dy;
      x += dx;
    }

    return hash;
  }

  Id id() const noexcept { return id_; }

  bool operator<(Picture const &pict) const noexcept { return id_ < pict.id_; }

  bool operator==(Picture const &pict) const noexcept {
    return id_ == pict.id_;
  }

  bool in_use() const noexcept { return in_use_; }
  void use() noexcept { in_use_ = true; }
  void release() noexcept { in_use_ = false; }

  friend std::ostream &operator<<(std::ostream &os, Picture const &pict);

  unsigned find_monsters() {
    for (unsigned r = 0; r < 4; ++r) {
      rotate();
      for (unsigned f = 0; f < 2; ++f) {
        flip();
        auto monster_count = find_monsters1();
        if (monster_count > 0) {
          return monster_count;
        }
      }
    }

    return 0;
  }

  unsigned roughness() const {
    unsigned rough = 0;
    for (auto const &r : rows_) {
      for (auto c : r) {
        if (c == '#') {
          ++rough;
        }
      }
    }
    return rough;
  }

private:
  unsigned find_monsters1() {
    // 0         1
    // 01234567890123456789
    //                   #
    // #    ##    ##    ###
    //  #  #  #  #  #  #
    static const std::string::size_type locs[] = {18,
                                                  std::string::npos,
                                                  0,
                                                  5,
                                                  6,
                                                  11,
                                                  12,
                                                  17,
                                                  18,
                                                  19,
                                                  std::string::npos,
                                                  1,
                                                  4,
                                                  7,
                                                  10,
                                                  13,
                                                  16,
                                                  std::string::npos,
                                                  std::string::npos};
    static const std::string::size_type mwidth = 20;
    static const std::size_t mheight = 3;

    unsigned monster_count = 0;
    for (std::size_t y = 0; y <= rows_.size() - mheight; ++y) {
      for (std::string::size_type x = 0; x <= rows_[y].size() - mwidth; ++x) {
        std::size_t cy = 0;
        std::string::size_type const *cx = locs;
        bool found = true;
        while (cy < mheight) {
          if (*cx == std::string::npos) {
            ++cy;
          } else if (rows_[y + cy][x + *cx] != '#') {
            found = false;
            break;
          }
          ++cx;
        }
        if (found) {
          ++monster_count;
          std::size_t cy = 0;
          std::string::size_type const *cx = locs;
          while (cy < mheight) {
            if (*cx == std::string::npos) {
              ++cy;
            } else {
              rows_[y + cy][x + *cx] = '*';
            }
            ++cx;
          }
        }
      }
    }

    return monster_count;
  }

private:
  Id id_;
  std::vector<std::string> rows_;
  bool in_use_;
};

std::ostream &operator<<(std::ostream &os, Picture const &pict) {
  for (auto const &r : pict.rows_) {
    os << r << '\n';
  }
  return os;
}

using Pictures = std::map<Id, Picture>;
using HashMap = std::multimap<Hash, Id>;
using Array = std::map<std::pair<unsigned, unsigned>, Id>;

struct PictureArray {
  void add(Picture &&pic) {
    auto id = pic.id();
    auto [it, success] = pictures_.insert(std::make_pair(id, std::move(pic)));
    assert(success);

    // Set up hash -> ID mapping
    Picture &picture = it->second;
    for (unsigned r = 0; r < 4; ++r) {
      for (unsigned f = 0; f < 2; ++f) {
        hash_map_.insert({picture.hash(Edge::Top), picture.id()});
        picture.flip();
      }
      picture.rotate();
    }
  }

  Id solve() {
    assert(pictures_.size() == 9 || pictures_.size() == 144);
    for (auto &kv : pictures_) {
      if (try_position(0, 0, kv.second)) {
        print_ids();
        return piece(0, 0).id() * piece(width() - 1, 0).id() *
               piece(0, height() - 1).id() *
               piece(width() - 1, height() - 1).id();
      }
    }

    assert(false);
    return 0;
  }
  unsigned width() const noexcept {
    if (pictures_.size() == 9) {
      return 3;
    } else if (pictures_.size() == 144) {
      return 12;
    } else {
      assert(false);
    }

    return 0;
  }

  unsigned height() const noexcept { return width(); }

  Picture const &piece(unsigned x, unsigned y) const {
    auto const &it = array_.find({x, y});
    assert(it != array_.end());
    auto const &itp = pictures_.find(it->second);
    assert(itp != pictures_.end());
    return itp->second;
  }

private:
  bool try_position(unsigned x, unsigned y, Picture &pict) {
    if (pict.in_use()) {
      return false;
    }

    auto [it, success] = array_.insert({{x, y}, pict.id()});
    if (!success) {
      it->second = pict.id();
    }
    pict.use();
    for (unsigned r = 0; r < 4; ++r) {
      pict.rotate();
      for (unsigned f = 0; f < 2; ++f) {
        pict.flip();

        if (x > 0) {
          if (piece(x - 1, y).hash(Edge::Right) != pict.hash(Edge::Left)) {
            continue;
          }
        }

        if (y > 0) {
          if (piece(x, y - 1).hash(Edge::Bottom) != pict.hash(Edge::Top)) {
            continue;
          }
        }

        auto next_x = x + 1;
        auto next_y = y;
        Hash hash = pict.hash(Edge::Right);

        if (next_x == width()) {
          next_x = 0;
          next_y = y + 1;
          hash = piece(0, y).hash(Edge::Bottom);
        }

        if (next_y == height()) {
          return true;
        }

        auto [it, ite] = hash_map_.equal_range(hash);
        while (it != ite) {
          auto itp = pictures_.find(it->second);
          assert(itp != pictures_.end());
          if (try_position(next_x, next_y, itp->second)) {
            return true;
          }
          ++it;
        }
      }
    }
    pict.release();

    return false;
  }

  void print_ids() const {
    for (unsigned y = 0; y < height(); ++y) {
      for (unsigned x = 0; x < width(); ++x) {
        std::cout << " " << piece(x, y).id();
      }
      std::cout << "\n";
    }
  }

  Pictures pictures_;
  HashMap hash_map_;
  Array array_;
};

Picture::Picture(PictureArray const &array) : id_(0), in_use_(false) {
  for (unsigned y = 0; y < array.height(); ++y) {
    auto ybase = rows_.size();
    for (unsigned x = 0; x < array.width(); ++x) {
      auto &pict = array.piece(x, y);
      for (auto py = 1; py < pict.rows_.size() - 1; ++py) {
        auto yidx = ybase + py - 1;
        if (rows_.size() <= yidx) {
          rows_.resize(yidx + 1);
        }
        rows_[yidx] += pict.rows_[py].substr(1, pict.rows_[py].size() - 2);
      }
    }
  }
  assert(rows_.size() == array.height() * 8);
  assert(rows_[0].size() == array.width() * 8);
}

int main(void) {
  PictureArray pictures_;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    Picture pic(line, std::cin);
    pictures_.add(std::move(pic));
  }

  auto solution = pictures_.solve();
  std::cout << "Product = " << solution << "\n";

  Picture merged(pictures_);
  auto monster_count = merged.find_monsters();
  std::cout << merged;
  std::cout << "Number of monsters: " << monster_count << "\n";
  std::cout << "Roughness: " << merged.roughness() << "\n";
  return 0;
}