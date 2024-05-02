#ifndef AMSL_EXPRESSION_HPP
#define AMSL_EXPRESSION_HPP

#include <utility>
#include <vector>
#include <optional>
#include <string>
#include <ranges>
#include "ptr_wrapper.hpp"
#include "analyzed_expression.hpp"

struct AnalyzerScope {
  std::vector<std::string> variable_declarations{};
};

struct AnalyzerState {
  std::vector<AnalyzerScope> scopes{};

  constexpr AnalyzerScope &current_scope() {
    return scopes.back();
  }

  constexpr std::size_t get_variable_ref_id(const std::string &name) {
    std::size_t ref_id{};
    for (const auto &scope: scopes | std::views::reverse) {
      auto it = std::find(scope.variable_declarations.begin(), scope.variable_declarations.end(), name);
      if (it == scope.variable_declarations.end())
        ref_id += scope.variable_declarations.size();
      else
        return ref_id + std::distance(it, scope.variable_declarations.end()) - 1;
    }
    return std::string::npos;
  }
};

struct Expression {
  constexpr virtual ~Expression() = default;

  [[nodiscard]] constexpr virtual ptr_wrapper<AnalyzedExpression> analyze(AnalyzerState &state) const = 0;
};

struct ExpressionList : public Expression {
  std::vector<ptr_wrapper<Expression>> expressions;

  constexpr explicit ExpressionList(std::vector<ptr_wrapper<Expression>> &&expressions = {}) : expressions{
    std::move(expressions)} {}

  [[nodiscard]] constexpr ptr_wrapper<AnalyzedExpression> analyze(AnalyzerState &state) const override {
    std::vector<ptr_wrapper<AnalyzedExpression>> analyzed_expressions;
    state.scopes.emplace_back();
    for (const auto &expression: expressions)
      analyzed_expressions.push_back(expression->analyze(state));
    state.scopes.pop_back();
    return make_ptr_wrapper<AnalyzedExpressionList>(std::move(analyzed_expressions));
  }
};

struct FunctionCallExpression : public Expression {
  std::string name;
  std::vector<ptr_wrapper<Expression>> parameters;

  constexpr explicit FunctionCallExpression(std::string name,
                                            std::vector<ptr_wrapper<Expression>> &&parameters = {}) : name{name},
                                                                                                      parameters{
                                                                                                        std::move(
                                                                                                          parameters)} {}

  [[nodiscard]] constexpr ptr_wrapper<AnalyzedExpression> analyze(AnalyzerState &state) const override {
    std::vector<ptr_wrapper<AnalyzedExpression>> analyzed_parameters;
    for (const auto &parameter: parameters) {
      state.scopes.emplace_back();
      analyzed_parameters.push_back(parameter->analyze(state));
      state.scopes.pop_back();
    }
    return make_ptr_wrapper<AnalyzedFunctionCallExpression>(name, std::move(analyzed_parameters));
  }
};

struct VariableDeclarationExpression : public Expression {
  std::string name{};
  std::optional<std::string> type{};
  std::optional<ptr_wrapper<Expression>> initializer{};

  constexpr explicit VariableDeclarationExpression(std::string name,
                                                   std::optional<std::string> type = std::nullopt,
                                                   std::optional<ptr_wrapper<Expression>> &&initializer = std::nullopt)
    : name{name}, type{type}, initializer{std::move(initializer)} {}

  [[nodiscard]] constexpr ptr_wrapper<AnalyzedExpression> analyze(AnalyzerState &state) const override {
    state.scopes.emplace_back();
    auto analyzed_initializer = initializer.has_value() ? std::optional{initializer.value()->analyze(state)}
                                                        : std::nullopt;
    state.scopes.pop_back();
    state.current_scope().variable_declarations.push_back(name);
    return make_ptr_wrapper<AnalyzedVariableDeclarationExpression>(type, std::move(analyzed_initializer));
  }
};

struct VariableExpression : public Expression {
  std::string name;

  constexpr explicit VariableExpression(std::string name) : name{name} {}

  [[nodiscard]] constexpr ptr_wrapper<AnalyzedExpression> analyze(AnalyzerState &state) const override {
    return make_ptr_wrapper<AnalyzedVariableExpression>(state.get_variable_ref_id(name));
  }
};

struct AssignmentExpression : public Expression {
  ptr_wrapper<Expression> lhs;
  ptr_wrapper<Expression> rhs;

  constexpr explicit AssignmentExpression(ptr_wrapper<Expression> &&lhs, ptr_wrapper<Expression> &&rhs) : lhs{
    std::move(lhs)}, rhs{std::move(rhs)} {}

  [[nodiscard]] constexpr ptr_wrapper<AnalyzedExpression> analyze(AnalyzerState &state) const override {
    state.scopes.emplace_back();
    auto analyzed_lhs = lhs->analyze(state);
    state.scopes.pop_back();
    state.scopes.emplace_back();
    auto analyzed_rhs = rhs->analyze(state);
    state.scopes.pop_back();
    return make_ptr_wrapper<AnalyzedAssignmentExpression>(std::move(analyzed_lhs), std::move(analyzed_rhs));
  }
};

template<typename T>
struct LiteralExpression : public Expression {
  T value;

  constexpr explicit LiteralExpression(const T &value) : value{value} {}

  [[nodiscard]] constexpr ptr_wrapper<AnalyzedExpression> analyze(AnalyzerState &state) const override {
    return make_ptr_wrapper<AnalyzedLiteralExpression<T>>(value);
  }
};

#endif // AMSL_EXPRESSION_HPP
