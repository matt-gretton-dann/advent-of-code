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
#include <vector>

using Value = unsigned long;
struct Cup
{
  Cup() : value_(0), next_(nullptr) {}

  Value value() const { return value_; }
  void value(Value v) { value_ = v; }
  Cup* next() const { return next_; }
  void next(Cup* n) { next_ = n; }

private:
  Value value_;
  Cup* next_;
};

struct Cups
{
  static constexpr std::size_t num_cups_ = 1000000;
  static constexpr std::size_t num_moves_ = 10000000;

  Cups(std::string const& s) : cups_(new Cup[num_cups_]), current_(cups_)
  {
    std::size_t idx = 0;
    for (auto c : s) {
      cups_[idx].value(c - '0');
      cups_[idx].next(cups_ + idx + 1);
      ++idx;
    }
    Value v = 10;
    while (idx < num_cups_) {
      cups_[idx].value(v++);
      cups_[idx].next(cups_ + idx + 1);
      ++idx;
    }
    cups_[num_cups_ - 1].next(cups_);
    assert(v == num_cups_ + 1);
  }

  ~Cups() { delete[] cups_; }

  void play()
  {
    for (std::size_t move = 0; move < num_moves_; ++move) {
      if (move % 1000 == 0) {
        std::cout << "A" << move << " " << current_ - cups_ << " " << current_->value() << " "
                  << result() << "\n";
        // print();
      }

      // Remove first three after current.
      Cup* rb = current_->next();
      Cup* re = rb->next()->next();
      current_->next(re->next());
      re->next(nullptr);

      // Work out the destination
      auto vm1 = current_->value();
      do {
        --vm1;
        if (vm1 == 0) {
          vm1 = num_cups_;
        }
      } while (vm1 == rb->value() || vm1 == rb->next()->value() ||
               vm1 == rb->next()->next()->value());

      // Where do we insert?  Note that we use the hack that all the values are
      // actually in an index and we know what the value is (roughly) based on
      // the index.
      Cup* ins = nullptr;
      if (vm1 < 10) {
        ins = cups_;
        while (ins->value() != vm1) {
          assert(ins != cups_ + 10);
          ++ins;
        }
      }
      else {
        ins = cups_ + vm1 - 1;
      }

      assert(ins->value() == vm1);

      // Insert the removed elements.
      re->next(ins->next());
      ins->next(rb);

      // Move to next element.
      current_ = current_->next();
    }
  }

  Value result() const
  {
    Cup* one = cups_;
    while (one->value() != 1) {
      ++one;
      assert(one != cups_ + num_cups_);
    }

    return one->next()->value() * one->next()->next()->value();
  }

  void print() const
  {
    std::cout << "State:";
    Cup const* c = current_;
    do {
      std::cout << " " << c->value();
      c = c->next();
    } while (c != current_);
    std::cout << " loop \n";
  }

private:
  Cup* cups_;
  Cup* current_;
};

int main(void)
{
  std::string line;
  std::getline(std::cin, line);
  Cups cups(line);
  cups.play();
  std::cout << "Result: " << cups.result() << "\n";
  return 0;
}