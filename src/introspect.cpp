#include <iostream>
#include <iomanip>
#include "lexer.hpp"
#include "string.hpp"
#include "parser.hpp"

int main() {
  #include SOURCE_FILE

  auto str = as_string_t([]() {
    auto tokens = Lexer{source}.tokenize();
    std::string str = "Tokens: " + int_to_string(tokens.size()) + "\n";
    for (const auto &token: tokens) {
      std::visit(Overload{
        [&str](IntLiteral value) { str += "IntLiteral(" + int_to_string(value.data) + ")"; },
        [&str](const StringLiteral &value) { str += "StringLiteral(" + escape(value.data) + ")"; },
        [&str](const std::string &value) { str += "string(" + escape(value) + ")"; },
      }, token);
      str += "\n";
    }
    return str;
  });
  std::cout << std::string_view{str.data.begin(), str.data.end()} << std::endl;

  auto tokens = Lexer{source}.tokenize();
  auto expression = Parser{tokens}.parse();

  return 0;
}