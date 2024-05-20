#ifndef AMSL_EXECUTOR_HPP
#define AMSL_EXECUTOR_HPP

#include "compiler.hpp"
#include "amsl.hpp"
#include "builtin_functions.hpp"
#include "utils.hpp"

template<typename T>
struct Executor;

template<typename Expression, typename... Expressions>
struct Executor<CompiledExpressionList<ParameterPack<Expression, Expressions...>>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static decltype(auto) operator()(LocalScopeArgs &&... args) {
    Executor<Expression>{}(std::forward<LocalScopeArgs>(args)...);
    return Executor<CompiledExpressionList<ParameterPack<Expressions...>>>{}(std::forward<LocalScopeArgs>(args)...);
  }
};

template<typename Expression>
struct Executor<CompiledExpressionList<ParameterPack<Expression>>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static decltype(auto) operator()(LocalScopeArgs &&... args) {
    return Executor<Expression>{}(std::forward<LocalScopeArgs>(args)...);
  }
};

template<>
struct Executor<CompiledExpressionList<ParameterPack<>>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static auto operator()(LocalScopeArgs &&... args) {
    return;
  }
};

template<typename Initializer, typename... Expressions>
struct Executor<CompiledExpressionList<ParameterPack<CompiledVariableDeclarationWithInitializerAutoTypeExpression<Initializer>, Expressions...>>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static decltype(auto) operator()(LocalScopeArgs &&... args) {
    auto value = Executor<Initializer>{}(std::forward<LocalScopeArgs>(args)...);
    return Executor<CompiledExpressionList<ParameterPack<Expressions...>>>{}(std::forward<LocalScopeArgs>(args)...,
                                                                             take_ref(value));
  }
};

template<typename Type, typename Initializer, typename... Expressions>
struct Executor<CompiledExpressionList<ParameterPack<CompiledVariableDeclarationWithInitializerExpression<Type, Initializer>, Expressions...>>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static decltype(auto) operator()(LocalScopeArgs &&... args) {
    Type value = Executor<Initializer>{}(std::forward<LocalScopeArgs>(args)...);
    return Executor<CompiledExpressionList<ParameterPack<Expressions...>>>{}(std::forward<LocalScopeArgs>(args)...,
                                                                             take_ref(value));
  }
};

template<typename Type, typename... Expressions>
struct Executor<CompiledExpressionList<ParameterPack<CompiledVariableDeclarationExpression<Type>, Expressions...>>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static decltype(auto) operator()(LocalScopeArgs &&... args) {
    Type value{};
    return Executor<CompiledExpressionList<ParameterPack<Expressions...>>>{}(std::forward<LocalScopeArgs>(args)...,
                                                                             take_ref(value));
  }
};

template<string_t Name, typename... Parameters>
struct Executor<CompiledFunctionCallExpression<Name, ParameterPack<Parameters...>>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static decltype(auto) operator()(LocalScopeArgs &&... args) {
    return BuiltinFunction<Name>{}(Executor<Parameters>{}(std::forward<LocalScopeArgs>(args)...)...);
  }
};

template<typename Lhs, typename Rhs>
struct Executor<CompiledAssignmentExpression<Lhs, Rhs>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static decltype(auto) operator()(LocalScopeArgs &&... args) {
    return Executor<Lhs>{}(std::forward<LocalScopeArgs>(args)...) = Executor<Rhs>{}(
      std::forward<LocalScopeArgs>(args)...);
  }
};

template<typename T, T Value>
struct Executor<CompiledLiteral<T, Value>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static auto operator()(LocalScopeArgs &&... args) {
    return Value;
  }
};

template<auto N, string_t<N> Value>
struct Executor<CompiledLiteral<string_t<N>, Value>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static auto operator()(LocalScopeArgs &&... args) {
    return std::string{Value.c_str()};
  }
};

template<std::size_t RefID>
struct Executor<CompiledVariableExpression<RefID>> {
  template<typename ... LocalScopeArgs>
  AMSL_INLINE static decltype(auto) operator()(LocalScopeArgs &&... args) {
    return get_last_nth_argument<RefID>(std::forward<LocalScopeArgs>(args)...);
  }
};

#endif // AMSL_EXECUTOR_HPP
