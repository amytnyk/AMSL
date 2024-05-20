#include <iostream>
#include <iomanip>
#include "lexer.hpp"
#include "string.hpp"
#include "parser.hpp"
#include "analyzer.hpp"
#include "encoder.hpp"
#include "compiler.hpp"

int main() {
  #include SOURCE_FILE

  auto introspection_str = as_string_t([]() {
    auto tokens = Lexer{source}.tokenize();
    auto expression = Parser{tokens}.parse();
    auto analyzed_expression = Analyzer{expression}.analyze();
    auto bytes = encode_to_bytes(analyzed_expression);

    auto generator = []() {
      auto tokens = Lexer{source}.tokenize();
      auto expression = Parser{tokens}.parse();
      auto analyzed_expression = Analyzer{expression}.analyze();
      return encode_to_bytes(analyzed_expression);
    };
    static constexpr auto byte_array = to_byte_array<10 * 1024 * 1024>(generator);
    using TB_AST = Compiler<byte_array.begin()>::compiled;

    std::string str = "Step 1 - Embedder\nSource code:\n";
    str += source;
    str += "\n\n";
    str += "Step 2 - Lexer\nTokens: [";
    for (std::size_t idx = 0; idx < tokens.size(); ++idx) {
      if (idx)
        str += ", ";
      std::visit(Overload{
        [&str](IntLiteral value) { str += "IntLiteral(" + int_to_string(value.data) + ")"; },
        [&str](const StringLiteral &value) { str += "StringLiteral(" + escape(value.data) + ")"; },
        [&str](const std::string &value) { str += "string(" + escape(value) + ")"; },
      }, tokens[idx]);
    }
    str += "]\n\n";
    str += "Step 3 - Parser\nAST: " + expression->as_string();
    str += "\n\n";
    str += "Step 4 - Analyzer\nAnalyzed AST: " + analyzed_expression->as_string();
    str += "\n\n";
    str += "Step 5 - Encoder\nByte vector: [";
    for (std::size_t idx = 0; idx < bytes.size(); ++idx) {
      if (idx)
        str += ", ";
      auto repr = int_to_string(static_cast<int>(bytes[idx]), IntBase::HEX);
      str += std::string{"0x"} + (repr.size() == 2 ? "" : "0") + repr;
    }
    str += "]";
    str += "\n\n";
    str += "Step 6 - Runtime to compile-time wall\nByte array: [";
    for (std::size_t idx = 0; idx < byte_array.size(); ++idx) {
      if (idx)
        str += ", ";
      auto repr = int_to_string(static_cast<int>(byte_array[idx]), IntBase::HEX);
      str += std::string{"0x"} + (repr.size() == 2 ? "" : "0") + repr;
    }
    str += "]";
    str += "\n\n";
    str += "Step 7 - Compiler\nTB-AST: ";
    str += get_type_name<TB_AST>();
    str += "\n\n";
    str += "Step 8 - Executor\nExecuting here ...";
    return str;
  });
  std::cout << std::string_view{introspection_str.data.begin(), introspection_str.data.end()} << std::endl;

  return 0;
}
