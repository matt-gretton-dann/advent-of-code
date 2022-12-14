//
// Created by Matthew Gretton-Dann on 14/12/2022.
//

#ifndef ADVENT_OF_CODE_POSITION_H
#define ADVENT_OF_CODE_POSITION_H

#include <compare>

template<typename Int>
struct Pos
{
  constexpr Pos() noexcept = default;
  constexpr Pos(Int x, Int y) noexcept : x_(x), y_(y) {}
  constexpr Pos(Pos const&) noexcept = default;
  auto constexpr operator=(Pos const&) noexcept -> Pos& = default;
  constexpr Pos(Pos&&) noexcept = default;
  auto constexpr operator=(Pos&&) noexcept -> Pos& = default;
  constexpr ~Pos() noexcept = default;

  [[nodiscard]] auto constexpr x() const noexcept -> Int { return x_; }
  auto constexpr x(Int x) noexcept { x_ = x; }
  [[nodiscard]] auto constexpr y() const noexcept -> Int { return y_; }
  auto constexpr y(Int y) noexcept { y_ = y; }

  auto constexpr operator+=(Pos const& r) noexcept -> Pos&
  {
    x_ += r.x_;
    y_ += r.y_;
    return *this;
  }

  auto constexpr operator<=>(Pos const& r) const noexcept -> std::strong_ordering
  {
    auto result{y_ <=> r.y_};
    if (result == std::strong_ordering::equal) {
      result = x_ <=> r.x_;
    }
    return result;
  }

  auto constexpr operator==(Pos const& r) const noexcept -> bool
  {
    return x_ == r.x_ && y_ == r.y_;
  }

private:
  Int x_{0};
  Int y_{0};
};

template<typename Int>
auto operator+(Pos<Int> const& x, Pos<Int> const& y) noexcept -> Pos<Int>
{
  auto z{x};
  z += y;
  return y;
}

#endif  // ADVENT_OF_CODE_POSITION_H
