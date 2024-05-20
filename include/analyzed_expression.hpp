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

  [[nodiscard]] constexpr virtual std::string as_string() const = 0;

protected:
  [[nodiscard]] constexpr virtual std::byte identifier() const = 0;

  constexpr virtual void encode_to_bytes(Bytes &bytes) const = 0;
};

class AnalyzedExpressionList : public AnalyzedExpression {
public:
  std::vector<ptr_wrapper<AnalyzedExpression>> expressions;

  constexpr explicit AnalyzedExpressionList(std::vector<ptr_wrapper<AnalyzedExpression>> &&expressions = {})
    : expressions{std::move(expressions)} {
  }

  [[nodiscard]] constexpr std::string as_string() const override {
    std::string str = "AnalyzedExpressionList(expressions=[";
    for (std::size_t idx = 0; idx < expressions.size(); ++idx) {
      if (idx)
        str += ", ";
      str += expressions[idx]->as_string();
    }
    return str + "])";
  }

protected:
  [[nodiscard]] constexpr std::byte identifier() const override { return std::byte{0}; }

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

  [[nodiscard]] constexpr std::string as_string() const override {
    std::string str = "AnalyzedFunctionCallExpression(name='" + name + "', parameters=[";
    for (std::size_t idx = 0; idx < parameters.size(); ++idx) {
      if (idx)
        str += ", ";
      str += parameters[idx]->as_string();
    }
    return str + "])";
  }

protected:
  [[nodiscard]] constexpr std::byte identifier() const override { return std::byte{1}; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, name);
    ::encode(bytes, parameters);
  }
};

class AnalyzedVariableDeclarationExpression : public AnalyzedExpression {
public:
  std::string type;

  constexpr explicit AnalyzedVariableDeclarationExpression(std::string type)
    : type{type} {}

  [[nodiscard]] constexpr std::string as_string() const override {
    return "AnalyzedVariableDeclarationExpression(type='" + type + "')";
  }

protected:
  [[nodiscard]] constexpr std::byte identifier() const override { return std::byte{2}; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, type);
  }
};

class AnalyzedVariableDeclarationWithInitializerExpression : public AnalyzedExpression {
public:
  std::string type;
  ptr_wrapper<AnalyzedExpression> initializer;

  constexpr explicit AnalyzedVariableDeclarationWithInitializerExpression(std::string type,
                                                                          ptr_wrapper<AnalyzedExpression> &&initializer)
    : type{type}, initializer{std::move(initializer)} {}

  [[nodiscard]] constexpr std::string as_string() const override {
    return "AnalyzedVariableDeclarationWithInitializerExpression(type='" + type + "', initializer=" +
           initializer->as_string() + ")";
  }

protected:
  [[nodiscard]] constexpr std::byte identifier() const override { return std::byte{3}; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, type);
    ::encode(bytes, initializer);
  }
};

class AnalyzedVariableDeclarationWithInitializerAutoTypeExpression : public AnalyzedExpression {
public:
  ptr_wrapper<AnalyzedExpression> initializer;

  constexpr explicit AnalyzedVariableDeclarationWithInitializerAutoTypeExpression(
    ptr_wrapper<AnalyzedExpression> &&initializer)
    : initializer{std::move(initializer)} {}

  [[nodiscard]] constexpr std::string as_string() const override {
    return "AnalyzedVariableDeclarationWithInitializerAutoTypeExpression(initializer=" +
           initializer->as_string() + ")";
  }

protected:
  [[nodiscard]] constexpr std::byte identifier() const override { return std::byte{4}; }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, initializer);
  }
};

class AnalyzedVariableExpression : public AnalyzedExpression {
public:
  std::size_t ref_id;

  constexpr explicit AnalyzedVariableExpression(std::size_t ref_id) : ref_id{ref_id} {}

  [[nodiscard]] constexpr std::string as_string() const override {
    return "AnalyzedVariableExpression(ref_id=" + int_to_string(ref_id) + ")";
  }

protected:
  [[nodiscard]] constexpr std::byte identifier() const override { return std::byte{5}; }

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

  [[nodiscard]] constexpr std::string as_string() const override {
    return "AnalyzedAssignmentExpression(lhs=" + lhs->as_string() + ", rhs=" + rhs->as_string() + ")";
  }

protected:
  [[nodiscard]] constexpr std::byte identifier() const override { return std::byte{6}; }

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

  [[nodiscard]] constexpr std::string as_string() const override {
    if constexpr (std::is_same_v<T, int>)
      return std::string{"AnalyzedLiteralExpression(value="} + int_to_string(value) + ")";
    else
      return std::string{"AnalyzedLiteralExpression(value='"} + value + "')";
  }

protected:
  [[nodiscard]] constexpr std::byte
  identifier() const override {
    if constexpr (std::is_same_v<T, int>)
      return std::byte{7};
    else if constexpr (std::is_same_v<T, std::string>)
      return std::byte{8};
  }

  constexpr void encode_to_bytes(Bytes &bytes) const override {
    ::encode(bytes, value);
  }
};

#endif // AMSL_ANALYZED_EXPRESSION_HPP
