#include <iostream>
#include <string>
#include <vector>

using UInt = unsigned long;

namespace {

auto bit(std::string const& stream, std::size_t idx) -> UInt
{
  std::string_view vals{"0123456789ABCDEF"};
  auto val{vals.find(stream[idx / 4])};
  assert(val != std::string::npos);
  val = (val >> (3 - (idx % 4))) & 1;
  return val;
}

auto read_number(std::string const& stream, std::size_t idx, std::size_t length) -> UInt
{
  UInt result{0};
  while (length-- != 0) {
    result <<= 1;
    result |= bit(stream, idx++);
  }
  return result;
}

struct Operator
{
  static auto build(std::string const& stream) -> Operator
  {
    auto [result, idx] = build(stream, 0);
    return result;
  }

  auto version_sum() const noexcept -> UInt
  {
    UInt version{version_};
    for (auto const& child : children_) {
      version += child.version_sum();
    }
    return version;
  }

private:
  static auto build(std::string const& stream, std::size_t idx) -> std::pair<Operator, std::size_t>
  {
    UInt version{read_number(stream, idx, 3)};
    idx += 3;
    UInt type{read_number(stream, idx, 3)};
    idx += 3;
    Operator result{version, type};
    if (type == 4) {
      UInt value{0};
      bool cont{false};
      do {
        cont = (bit(stream, idx++) == 1);
        value <<= 4;
        value |= read_number(stream, idx, 4);
        idx += 4;
      } while (cont);
      result.value_ = value;
    }
    else {
      bool count_subpackets{bit(stream, idx++) == 1};
      if (count_subpackets) {
        UInt count{read_number(stream, idx, 11)};
        idx += 11;
        while (count-- != 0) {
          auto [subpacket, new_idx] = build(stream, idx);
          result.children_.push_back(subpacket);
          idx = new_idx;
        }
      }
      else {
        UInt size{read_number(stream, idx, 15)};
        idx += 15;
        auto target_idx{idx + size};
        while (idx < target_idx) {
          auto [subpacket, new_idx] = build(stream, idx);
          result.children_.push_back(subpacket);
          idx = new_idx;
        }
      }
    }

    return {result, idx};
  }

  Operator(UInt version, UInt type) : version_(version), type_(type) {}

  UInt version_;
  UInt type_;
  UInt value_{0};
  std::vector<Operator> children_;
};
}  // namespace

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line\n";
    return 1;
  }

  auto operators{Operator::build(line)};
  std::cout << "Version sums: " << operators.version_sum() << '\n';
  return 0;
}
