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

enum class Token : char { Eof, Number, LParens = '(', RParens = ')', Add = '+', Multiply = '*' };
using Value = unsigned long;

struct Parser
{
  Parser(std::string const& s) : expr_(s), pos_(0) { skip_whitespace(); }

  Value evaluate() { return binop(); }

private:
  Value binop()
  {
    auto value = primary();
    do {
      if (peek() == Token::Add) {
        chew(Token::Add);
        value += primary();
      }
      else if (peek() == Token::Multiply) {
        chew(Token::Multiply);
        value *= primary();
      }
      else {
        return value;
      }
    } while (true);
  }

  Value primary()
  {
    if (peek() == Token::LParens) {
      chew(Token::LParens);
      Value value = binop();
      chew(Token::RParens);
      return value;
    }
    else if (peek() == Token::Number) {
      return chew_number();
    }
    else {
      std::cout << "expr_ = " << expr_ << "\n";
      std::cout << "pos_ = " << pos_ << "\n";
      std::cout << "End = " << expr_.substr(pos_) << "\n";
      abort();
    }
  }

  Token peek()
  {
    if (pos_ == expr_.size()) {
      return Token::Eof;
    }
    switch (expr_[pos_]) {
    case '(':
      return Token::LParens;
    case ')':
      return Token::RParens;
    case '+':
      return Token::Add;
    case '*':
      return Token::Multiply;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return Token::Number;
    default:
      std::cout << "expr_ = " << expr_ << "\n";
      std::cout << "pos_ = " << pos_ << "\n";
      std::cout << "End = " << expr_.substr(pos_) << "\n";
      abort();
    }
  }

  void chew(Token tok)
  {
    assert(peek() == tok);
    switch (tok) {
    case Token::LParens:
    case Token::RParens:
    case Token::Add:
    case Token::Multiply:
      ++pos_;
      skip_whitespace();
      break;
    default:
      assert(false);
    }
  }

  void skip_whitespace()
  {
    while (pos_ < expr_.size() && expr_[pos_] == ' ') {
      ++pos_;
    }
  }

  Value chew_number()
  {
    assert(peek() == Token::Number);

    std::size_t len = 0;
    Value value = std::stoul(expr_.substr(pos_), &len);
    pos_ += len;
    skip_whitespace();
    return value;
  }

  std::string expr_;
  std::string::size_type pos_;
};

int main(void)
{
  std::string line;
  Value result = 0;
  while (std::getline(std::cin, line)) {
    Parser p(line);
    Value r = p.evaluate();
    std::cout << line << " = " << r << "\n";
    result += r;
  }

  std::cout << "Sum: " << result << "\n";

  return 0;
}