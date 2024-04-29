#ifndef AMSL_PARSER_HPP
#define AMSL_PARSER_HPP

#include <vector>
#include "token.hpp"
#include "expression.hpp"
#include "utils.hpp"

struct ParserState {
  std::size_t current_token{};
};

class Parser {
public:
  constexpr explicit Parser(const std::vector<Token> &tokens) : tokens{tokens} {}

  constexpr ptr_wrapper<Expression> parse() {
    state = ParserState{};
    return parse_expression();
  }

  constexpr const Token &get_next_token() {
    return tokens[state.current_token];
  }

  constexpr const Token &fetch_token() {
    return tokens[state.current_token++];
  }

  constexpr ptr_wrapper<FunctionCallExpression> parse_function_call_expression() {
    const auto &name = std::get<std::string>(fetch_token());
    fetch_token();

    auto expression = make_ptr_wrapper<FunctionCallExpression>(name);
    while (true) {
      const auto &next_token = get_next_token();
      if (std::holds_alternative<std::string>(next_token) && std::get<std::string>(next_token) == ")") {
        fetch_token();
        break;
      }
      expression->parameters.push_back(parse_expression());
    }
    return expression;
  }

  constexpr ptr_wrapper<VariableDeclarationExpression> parse_variable_declaration_expression() {
    const auto &name = std::get<std::string>(fetch_token());

    std::optional<std::string> type{};

    const auto &next_token = get_next_token();
    if (std::holds_alternative<std::string>(next_token) && std::get<std::string>(next_token) == ":") {
      fetch_token();
      type = std::get<std::string>(fetch_token());
    }

    ptr_wrapper<Expression> initializer{};
    const auto &next_token2 = get_next_token();
    if (std::holds_alternative<std::string>(next_token2) && std::get<std::string>(next_token2) == "=") {
      fetch_token();
      initializer = parse_expression();
    }

    return make_ptr_wrapper<VariableDeclarationExpression>(name, type, std::move(initializer));
  }

  constexpr ptr_wrapper<AssignmentExpression> parse_assignment_expression() {
    auto lhs = parse_expression();
    fetch_token();
    auto rhs = parse_expression();
    return make_ptr_wrapper<AssignmentExpression>(std::move(lhs), std::move(rhs));
  }

  constexpr ptr_wrapper<ExpressionList> parse_expression_list() {
    auto expression = make_ptr_wrapper<ExpressionList>();
    while (true) {
      const auto &next_token = get_next_token();
      if (std::holds_alternative<std::string>(next_token) && std::get<std::string>(next_token) == "}") {
        fetch_token();
        break;
      }
      auto expression_item = parse_expression();
      if (expression_item)
        expression->expressions.push_back(std::move(expression_item));
    }
    return expression;
  }

  constexpr ptr_wrapper<Expression> parse_expression() {
    return std::visit<ptr_wrapper<Expression>>(Overload{
      [](IntLiteral value) {
        return make_ptr_wrapper<LiteralNodeExpression<int>>(value.data);
      },
      [](const StringLiteral &value) { return make_ptr_wrapper<LiteralNodeExpression<std::string>>(value.data); },
      [this](const std::string &value) -> ptr_wrapper<Expression> {
        if (value == ";")
          return nullptr;
        else if (value == "{")
          return parse_expression_list();
        else if (value == "@")
          return parse_function_call_expression();
        else if (value == "let")
          return parse_variable_declaration_expression();
        else if (value == "apply")
          return parse_assignment_expression();
        else
          return make_ptr_wrapper<VariableExpression>(value);
      },
    }, fetch_token());
  }

private:
  const std::vector<Token> &tokens;
  ParserState state{};
};

#endif //AMSL_PARSER_HPP
