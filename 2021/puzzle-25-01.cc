#include <cassert>
#include <array>
#include <iostream>
#include <regex>
#include <string>

struct State
{
  void add_line(std::string const& line)
  {
    assert(lines_.empty() || lines_[0].size() == line.size());
    lines_.push_back(line);
  }

  void print(std::ostream& os) const
  {
    for (auto const& line : lines_) {
      os << line << '\n';
    }
  }

  [[nodiscard]] auto move_right() const noexcept -> State
  {
    State result{*this};
    for (std::size_t row{0}; row < lines_.size(); ++row) {
      for (std::size_t col{0}; col < lines_[row].size(); ++col) {
        auto from = lines_[row][col];
        auto to = lines_[row][(col + 1) % lines_[row].size()];
        if (from == '>' && to == '.') {
          result.lines_[row][(col + 1) % lines_[row].size()] = '>';
          result.lines_[row][col] = '.';
        }
      }
    }
    return result;
  }

  [[nodiscard]] auto move_down() const noexcept -> State
  {
    State result{*this};
    for (std::size_t row{0}; row < lines_.size(); ++row) {
      auto const& from_row = lines_[row];
      auto const& to_row = lines_[(row + 1) % lines_.size()];
      for (std::size_t col{0}; col < from_row.size(); ++col) {
        if (from_row[col] == 'v' && to_row[col] == '.') {
          result.lines_[(row + 1) % lines_.size()][col] = 'v';
          result.lines_[row][col] = '.';
        }
      }
    }

    return result;
  }

  auto operator==(State const& rhs) const noexcept -> bool { return lines_ == rhs.lines_; }

private:
  std::vector<std::string> lines_;
};

auto main() -> int
{
  std::string line;
  State state;

  while (std::getline(std::cin, line) && !line.empty()) {
    state.add_line(line);
  }

  std::size_t count{0};
  while (true) {
    State s1 = state.move_right().move_down();
    ++count;
    if (s1 == state) {
      state.print(std::cout);
      std::cout << "Done in " << count << " steps\n";
      return 0;
    }

    state = s1;
  }
}
