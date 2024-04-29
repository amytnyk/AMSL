#ifndef AMSL_EXPRESSION_HPP
#define AMSL_EXPRESSION_HPP

#include <utility>
#include <vector>
#include <optional>
#include <string>
#include "ptr_wrapper.hpp"

struct Expression {
  constexpr virtual ~Expression() = default;
};

struct ExpressionList : public Expression {
  std::vector<ptr_wrapper<Expression>> expressions;

  constexpr explicit ExpressionList(std::vector<ptr_wrapper<Expression>> &&expressions = {}) : expressions{
    std::move(expressions)} {}
};

struct FunctionCallExpression : public Expression {
  std::string name;
  std::vector<ptr_wrapper<Expression>> parameters;

  constexpr explicit FunctionCallExpression(std::string name,
                                            std::vector<ptr_wrapper<Expression>> &&parameters = {}) : name{
    std::move(name)}, parameters{std::move(parameters)} {}
};

struct VariableDeclarationExpression : public Expression {
  std::string name;
  std::optional<std::string> type;
  ptr_wrapper<Expression> initializer;

  constexpr explicit VariableDeclarationExpression(std::string name,
                                                   const std::optional<std::string> &type = std::nullopt,
                                                   ptr_wrapper<Expression> &&initializer = nullptr)
    : name{std::move(name)}, type{type}, initializer{std::move(initializer)} {}
};

struct VariableExpression : public Expression {
  std::string name;

  constexpr explicit VariableExpression(std::string name) : name{std::move(name)} {}
};

struct AssignmentExpression : public Expression {
  ptr_wrapper<Expression> lhs;
  ptr_wrapper<Expression> rhs;

  constexpr explicit AssignmentExpression(ptr_wrapper<Expression> &&lhs, ptr_wrapper<Expression> &&rhs) : lhs{
    std::move(lhs)}, rhs{std::move(rhs)} {}
};

template<typename T>
struct LiteralExpression : public Expression {
  T value;

  constexpr explicit LiteralExpression(const T &value) : value{value} {}
};

#endif //AMSL_EXPRESSION_HPP
