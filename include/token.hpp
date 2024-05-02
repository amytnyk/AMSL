#ifndef AMSL_TOKEN_HPP
#define AMSL_TOKEN_HPP

#include <variant>
#include <string>

template<typename T>
struct Literal {
  T data{};
};

using IntLiteral = Literal<int>;
using StringLiteral = Literal<std::string>;

using Token = std::variant<IntLiteral, StringLiteral, std::string>;

#endif // AMSL_TOKEN_HPP
