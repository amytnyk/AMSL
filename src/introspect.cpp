#include <iostream>
#include <iomanip>
#include "lexer.hpp"
#include "string.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "encoder.hpp"

template<typename T, auto MaxSize>
struct oversized_array {
  std::array<T, MaxSize> data{};
  std::size_t size{};
};

template<auto MaxSize, typename T>
consteval auto to_oversized_array(const std::vector<T> &bytes) {
  oversized_array<T, MaxSize> result{};
  std::copy(bytes.begin(), bytes.end(), result.data.begin());
  result.size = bytes.size();
  return result;
}

template<auto MaxSize>
consteval auto to_right_sized_array(auto generator) {
  constexpr auto oversized = to_oversized_array<MaxSize>(generator());
  std::array<typename decltype(oversized.data)::value_type, oversized.size> result;
  std::copy(oversized.data.begin(), std::next(oversized.data.begin(), oversized.size), result.begin());
  return result;
}

template<auto MaxSize>
consteval auto to_byte_array(auto generator) {
  return to_right_sized_array<MaxSize>(generator);
}

int main() {
  #include SOURCE_FILE

  auto tokens_str = as_string_t([]() {
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
  std::cout << std::string_view{tokens_str.data.begin(), tokens_str.data.end()} << std::endl;

  constexpr auto generator = []() {
    auto tokens = Lexer{source}.tokenize();
    auto expression = Parser{tokens}.parse();
    auto analyzer_expression = Analyzer{expression}.analyze();
    return encode_to_bytes(analyzer_expression);
  };
  constexpr auto byte_array = to_byte_array<10 * 1024 * 1024>(generator);

  return 0;
}
