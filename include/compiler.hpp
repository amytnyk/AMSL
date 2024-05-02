#ifndef AMSL_COMPILER_HPP
#define AMSL_COMPILER_HPP

#include "string.hpp"
#include "traits.hpp"
#include <bit>
#include <span>

template<typename ExpressionPack>
struct CompiledExpressionList {
  using expressions = ExpressionPack;
};

template<string_t Name, typename ParameterPack>
struct CompiledFunctionCallExpression {
  static constexpr auto name = Name;
  using parameters = ParameterPack;
};

template<typename Type, typename Initializer>
struct CompiledVariableDeclarationExpression {
  using type = Type;
  using initializer = Initializer;
};

template<std::size_t RefID>
struct CompiledVariableExpression {
  static constexpr auto ref_id = RefID;
};

template<typename Lhs, typename Rhs>
struct CompiledAssignmentExpression {
  using lhs = Lhs;
  using rhs = Rhs;
};

template<typename T, T Value>
struct CompiledLiteral {
  using type = T;
  static constexpr auto value = Value;
};

template<auto Span>
constexpr auto span_to_array() {
  std::array<std::byte, Span.size()> result;
  std::copy(Span.begin(), Span.end(), result.begin());
  return result;
}

template<auto Ptr, typename T>
struct Decoder;

template<auto Ptr, Trivial T>
struct Decoder<Ptr, T> {
  static constexpr auto decoded = std::bit_cast<sizeof(T)>(
    span_to_array<std::span<std::byte, sizeof(T)>{Ptr, sizeof(T)}>());
};

template<auto Ptr>
struct Compiler;

template<auto Ptr> requires (*Ptr == 5)
struct Compiler<Ptr> {
  static constexpr auto compiled = Decoder<Ptr, int>::decoded;
};

#endif // AMSL_COMPILER_HPP
