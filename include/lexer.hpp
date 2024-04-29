#ifndef AMSL_LEXER_HPP
#define AMSL_LEXER_HPP

#include <vector>
#include <array>
#include "token.hpp"
#include "utils.hpp"

struct LexerState {
  std::size_t current_idx{};
  std::string raw_token{};
  bool quoted{};
  bool escaped{};
  std::vector<Token> tokens{};
};

class Lexer {
public:
  constexpr explicit Lexer(std::string_view str) : str{str} {}

  constexpr std::vector<Token> tokenize() {
    state = LexerState{};
    while (state.current_idx < str.size())
      proceed();
    push_token();
    return state.tokens;
  }

private:
  constexpr void proceed() {
    if (state.current_idx >= str.size())
      return;

    char chr = str[state.current_idx++];

    if (state.escaped) {
      if (chr == 'n')
        state.raw_token += '\n';
      else if (chr == 't')
        state.raw_token += '\t';
      else if (chr == 'r')
        state.raw_token += '\r';
      else if (chr == 'a')
        state.raw_token += '\a';
      else if (chr == 'f')
        state.raw_token += '\f';
      else if (chr == 'v')
        state.raw_token += '\v';
      else if (chr == 'b')
        state.raw_token += '\b';
      else
        state.raw_token += chr;
      state.escaped = false;
    } else if (chr == '\"') {
      push_token();
      state.quoted = !state.quoted;
    } else if (state.quoted) {
      if (chr == '\\')
        state.escaped = true;
      else
        state.raw_token += chr;
    } else if (is_whitespace(chr)) {
      push_token();
    } else if (std::find(delimiters.begin(), delimiters.end(), chr) != delimiters.end()) {
      push_token();
      state.raw_token = chr;
      push_token();
    } else {
      state.raw_token += chr;
    }
  }

  constexpr void push_token() {
    if (state.raw_token.empty())
      return;

    state.tokens.push_back(parse_raw_token());
    state.raw_token.clear();
  }

  constexpr Token parse_raw_token() {
    if (state.quoted)
      return StringLiteral{state.raw_token};
    else if (state.raw_token.starts_with(BINARY_PREFIX) && state.raw_token.size() > BINARY_PREFIX.size())
      return IntLiteral{string_to_int(state.raw_token.substr(BINARY_PREFIX.size()), IntBase::BINARY)};
    else if (state.raw_token.starts_with(DECIMAL_PREFIX) && state.raw_token.size() > DECIMAL_PREFIX.size())
      return IntLiteral{string_to_int(state.raw_token.substr(DECIMAL_PREFIX.size()), IntBase::DECIMAL)};
    else if (state.raw_token.starts_with(HEX_PREFIX) && state.raw_token.size() > HEX_PREFIX.size())
      return IntLiteral{string_to_int(state.raw_token.substr(HEX_PREFIX.size()), IntBase::HEX)};
    else if (state.raw_token.starts_with(OCTAL_PREFIX) && state.raw_token.size() > OCTAL_PREFIX.size())
      return IntLiteral{string_to_int(state.raw_token.substr(OCTAL_PREFIX.size()), IntBase::OCTAL)};
    else if (convertible_to_int_decimal(state.raw_token))
      return IntLiteral{string_to_int(state.raw_token, IntBase::DECIMAL)};
    else
      return state.raw_token;
  }

  static constexpr std::string_view BINARY_PREFIX{"0b"};
  static constexpr std::string_view DECIMAL_PREFIX{"0d"};
  static constexpr std::string_view OCTAL_PREFIX{"0"};
  static constexpr std::string_view HEX_PREFIX{"0x"};

  static constexpr auto delimiters = std::to_array(
    {';', ':', ',', '.', '-', '+', '*', '/', '(', ')', '[', ']', '{', '}', '<', '>', '@', '&', '|', '=', '!'});

  std::string_view str{};
  LexerState state{};
};

#endif //AMSL_LEXER_HPP
