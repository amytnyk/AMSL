#include <iostream>
#include <iomanip>
#include "lexer.hpp"
#include "string.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "encoder.hpp"
#include "compiler.hpp"

template <typename T>
constexpr auto get_type_name() -> std::string_view
{
#if defined(__clang__)
  constexpr auto prefix = std::string_view{"[T = "};
    constexpr auto suffix = "]";
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
  constexpr auto prefix = std::string_view{"with T = "};
  constexpr auto suffix = "; ";
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
  constexpr auto prefix = std::string_view{"get_type_name<"};
    constexpr auto suffix = ">(void)";
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
# error Unsupported compiler
#endif

  const auto start = function.find(prefix) + prefix.size();
  const auto end = function.find(suffix);
  const auto size = end - start;

  return function.substr(start, size);
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

  static constexpr auto byte_array = to_byte_array<10 * 1024 * 1024>(generator);
  using p = Compiler<byte_array.begin()>::compiled;
  std::cout << get_type_name<p>();
//  Executor<p>{}.execute();

  return 0;
}
