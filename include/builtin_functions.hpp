#ifndef AMSL_BUILTIN_FUNCTIONS_HPP
#define AMSL_BUILTIN_FUNCTIONS_HPP

#include <iostream>
#include <cmath>
#include <thread>
#include "string.hpp"
#include "utils.hpp"

template<string_t Name>
struct BuiltinFunction;

template<>
struct BuiltinFunction<"print"> {
  template<typename... Args>
  static constexpr void operator()(Args &&... args) {
    (std::cout << ... << args);
  }
};

template<>
struct BuiltinFunction<"println"> {
  template<typename... Args>
  static constexpr void operator()(Args &&... args) {
    (std::cout << ... << args) << std::endl;
  }
};

template<>
struct BuiltinFunction<"squared"> {
  static constexpr auto operator()(auto value) {
    return value * value;
  }
};

template<>
struct BuiltinFunction<"inc"> {
  static constexpr decltype(auto) operator()(auto &value) {
    return ++value;
  }
};

template<>
struct BuiltinFunction<"dec"> {
  static constexpr decltype(auto) operator()(auto &value) {
    return --value;
  }
};

template<>
struct BuiltinFunction<"pinc"> {
  static constexpr decltype(auto) operator()(auto &value) {
    return value++;
  }
};

template<>
struct BuiltinFunction<"pdec"> {
  static constexpr decltype(auto) operator()(auto &value) {
    return value--;
  }
};

template<>
struct BuiltinFunction<"add"> {
  static constexpr auto operator()(auto lhs, auto rhs) {
    return lhs + rhs;
  }
};

template<>
struct BuiltinFunction<"sub"> {
  static constexpr auto operator()(auto lhs, auto rhs) {
    return lhs - rhs;
  }
};

template<>
struct BuiltinFunction<"mul"> {
  static constexpr auto operator()(auto lhs, auto rhs) {
    return lhs * rhs;
  }
};

template<>
struct BuiltinFunction<"div"> {
  static constexpr auto operator()(auto lhs, auto rhs) {
    return lhs / rhs;
  }
};

template<>
struct BuiltinFunction<"pow"> {
  static constexpr auto operator()(auto value, auto power) {
    return std::pow(value, power);
  }
};

template<>
struct BuiltinFunction<"get_current_time"> {
  static auto operator()() {
    return get_current_time_fenced();
  }
};

template<>
struct BuiltinFunction<"get_millis"> {
  static auto operator()(auto value) {
    return to_ms(value);
  }
};

template<>
struct BuiltinFunction<"sleep"> {
  static auto operator()(auto value) {
    return std::this_thread::sleep_for(std::chrono::milliseconds{value});
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
