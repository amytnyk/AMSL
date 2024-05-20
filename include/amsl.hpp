#ifndef AMSL_AMSL_HPP
#define AMSL_AMSL_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <variant>
#include "string.hpp"
#include "executor.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "analyzer.hpp"

using namespace std::literals;

class AMSL {
public:
  template<string_t source_code>
  AMSL_INLINE auto execute() {
    return generate_executor<source_code>()();
  }

private:
  template<string_t source_code>
  consteval static auto generate_executor() {
    constexpr auto generator = []() {
      auto tokens = Lexer{source_code.sv()}.tokenize();
      auto expression = Parser{tokens}.parse();
      auto analyzer_expression = Analyzer{expression}.analyze();
      return encode_to_bytes(analyzer_expression);
    };
    static constexpr auto byte_array = to_byte_array<max_code_size>(generator);
    return Executor<typename Compiler<byte_array.begin()>::compiled>{};
  }

  static constexpr std::size_t max_code_size = 10 * 1024 * 1024; // 10 MB
};

#endif // AMSL_AMSL_HPP
