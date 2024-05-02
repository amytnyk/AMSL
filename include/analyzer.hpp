#ifndef AMSL_ANALYZER_HPP
#define AMSL_ANALYZER_HPP

#include "expression.hpp"
#include "analyzed_expression.hpp"

class Analyzer {
public:
  constexpr explicit Analyzer(const ptr_wrapper<Expression> &expression) : root{expression} {}

  constexpr ptr_wrapper<AnalyzedExpression> analyze() {
    AnalyzerState state;
    return root->analyze(state);
  }

private:
  const ptr_wrapper<Expression> &root;
};

#endif // AMSL_ANALYZER_HPP
