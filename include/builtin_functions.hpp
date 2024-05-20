#ifndef AMSL_BUILTIN_FUNCTIONS_HPP
#define AMSL_BUILTIN_FUNCTIONS_HPP

#include <iostream>
#include "string.hpp"

template<string_t Name>
struct BuiltinFunction;

template<>
struct BuiltinFunction<"print"> {
  template<typename... Args>
  static constexpr decltype(auto) operator()(Args &&... args) {
    return (std::cout << ... << args);
  }
};

template<>
struct BuiltinFunction<"println"> {
  template<typename... Args>
  static constexpr decltype(auto) operator()(Args &&... args) {
    return (std::cout << ... << args) << std::endl;
  }
};

template<>
struct BuiltinFunction<"squared"> {
  static constexpr auto operator()(auto value) {
    return value * value;
  }
};

template<>
struct BuiltinFunction<"readline"> {
  static auto operator()() {
    std::string str;
    std::getline(std::cin, str);
    return str;
  }
};

#endif // AMSL_BUILTIN_FUNCTIONS_HPP
