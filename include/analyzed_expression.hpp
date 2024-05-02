#ifndef AMSL_ANALYZED_EXPRESSION_HPP
#define AMSL_ANALYZED_EXPRESSION_HPP

#include <utility>
#include <vector>
#include <optional>
#include <string>
#include "ptr_wrapper.hpp"
#include "bytes.hpp"
#include "encodable.hpp"
#include "encoder.hpp"

class AnalyzedExpression : public Encodable {
public:
  constexpr virtual ~AnalyzedExpression() = default;

  constexpr void encode(Bytes &bytes) const override {
    ::encode(bytes, identifier());
    encode_to_bytes(bytes);
  }

protected:
  [[nodiscard]] constexpr virtual std::size_t identifier() const = 0;

  constexpr virtual void encode_to_bytes(Bytes &bytes) const = 0;
};

class AnalyzedExpressionList : public AnalyzedExpression {
public:
  std::vector<ptr_wrapper<AnalyzedExpression>> expressions;

  constexpr explicit AnalyzedExpressionList(std::vector<ptr_wrapper<AnalyzedExpression>> &&expressions = {})
    : expressions{std::move(expressions)} {
  }

protected:
  [[nodiscard]] constexpr std::size_t identifier() const override { return 0; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, expressions);
  }
};

class AnalyzedFunctionCallExpression : public AnalyzedExpression {
public:
  std::string name;
  std::vector<ptr_wrapper<AnalyzedExpression>> parameters;

  constexpr explicit AnalyzedFunctionCallExpression(std::string name,
                                                    std::vector<ptr_wrapper<AnalyzedExpression>> &&parameters = {})
    : name{name}, parameters{std::move(parameters)} {}

protected:
  [[nodiscard]] constexpr std::size_t identifier() const override { return 1; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, name);
    ::encode(bytes, parameters);
  }
};

class AnalyzedVariableDeclarationExpression : public AnalyzedExpression {
public:
  std::optional<std::string> type;
  std::optional<ptr_wrapper<AnalyzedExpression>> initializer;

  constexpr explicit AnalyzedVariableDeclarationExpression(const std::optional<std::string> &type = std::nullopt,
                                                           std::optional<ptr_wrapper<AnalyzedExpression>> &&initializer = std::nullopt)
    : type{type}, initializer{std::move(initializer)} {}

protected:
  [[nodiscard]] constexpr std::size_t identifier() const override { return 2; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, type);
    ::encode(bytes, initializer);
  }
};

class AnalyzedVariableExpression : public AnalyzedExpression {
public:
  std::size_t ref_id;

  constexpr explicit AnalyzedVariableExpression(std::size_t ref_id) : ref_id{ref_id} {}

protected:
  [[nodiscard]] constexpr std::size_t identifier() const override { return 3; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, ref_id);
  }
};

class AnalyzedAssignmentExpression : public AnalyzedExpression {
public:
  ptr_wrapper<AnalyzedExpression> lhs;
  ptr_wrapper<AnalyzedExpression> rhs;

  constexpr explicit AnalyzedAssignmentExpression(ptr_wrapper<AnalyzedExpression> &&lhs,
                                                  ptr_wrapper<AnalyzedExpression> &&rhs) : lhs{std::move(lhs)},
                                                                                           rhs{std::move(rhs)} {}

protected:
  [[nodiscard]] constexpr std::size_t identifier() const override { return 4; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, lhs);
    ::encode(bytes, rhs);
  }
};

template<typename T>
class AnalyzedLiteralExpression : public AnalyzedExpression {
public:
  T value;

  constexpr explicit AnalyzedLiteralExpression(const T &value) : value{value} {}

protected:
  [[nodiscard]] constexpr std::size_t identifier() const override { return 5; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, value);
  }
};

//template<typename T>
//constexpr std::byte AnalyzedLiteralExpression<T>::identifier{10};

#endif // AMSL_ANALYZED_EXPRESSION_HPP
