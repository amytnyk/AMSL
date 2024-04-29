#ifndef AMSL_COMPILER_HPP
#define AMSL_COMPILER_HPP


#include "string.hpp"

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

struct Compiler {
public:

};

#endif //AMSL_COMPILER_HPP
