#ifndef AMSL_UTILS_HPP
#define AMSL_UTILS_HPP

#include <cstdint>
#include <utility>
#include <string_view>
#include <cctype>
#include <chrono>
#include <algorithm>
#include <atomic>
#include "string.hpp"

#define AMSL_INLINE inline __attribute__((always_inline))

template<std::size_t N, typename... Args>
struct get_nth_argument_helper {
  static_assert(N < sizeof...(Args), "Index out of range");
};

template<std::size_t N, typename T, typename... Ts>
struct get_nth_argument_helper<N, T, Ts...> : get_nth_argument_helper<N - 1, Ts...> {
  constexpr static AMSL_INLINE decltype(auto) get(T &&value, Ts &&... args) {
    return get_nth_argument_helper<N - 1, Ts...>::get(args...);
  }
};

template<typename T, typename... Ts>
struct get_nth_argument_helper<0, T, Ts...> {
  constexpr static AMSL_INLINE decltype(auto) get(T &&value, Ts &&... args) {
    return value;
  }
};

template<std::size_t I, typename... Ts>
constexpr AMSL_INLINE decltype(auto) get_nth_argument(Ts &&... args) {
  return get_nth_argument_helper<I, Ts...>::get(std::forward<Ts>(args)...);
}

template<std::size_t I, typename... Ts>
constexpr AMSL_INLINE decltype(auto) get_last_nth_argument(Ts &&... args) {
  return get_nth_argument<sizeof...(Ts) - I - 1, Ts...>(std::forward<Ts>(args)...);
}

template<typename... Ts>
struct Overload : Ts ... {
  using Ts::operator()...;
};
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

enum class IntBase {
  BINARY = 2,
  OCTAL = 8,
  DECIMAL = 10,
  HEX = 16
};

constexpr char to_lower(char chr) {
  return ('A' <= chr && chr <= 'Z') ? static_cast<char>(chr - 'A' + 'a') : chr;
}

constexpr int char_to_int(char chr, IntBase base = IntBase::DECIMAL) {
  chr = to_lower(chr);
  if (base == IntBase::HEX) {
    if (chr == 'a')
      return 10;
    else if (chr == 'b')
      return 11;
    else if (chr == 'c')
      return 12;
    else if (chr == 'd')
      return 13;
    else if (chr == 'e')
      return 14;
    else if (chr == 'f')
      return 15;
  }
  return chr - '0';
}

constexpr int int_to_char(int value, IntBase base = IntBase::DECIMAL) {
  if (base == IntBase::HEX) {
    if (value >= 10)
      return 'a' + (value - 10);
  }
  return '0' + value;
}

constexpr int string_to_int(std::string_view str, IntBase base = IntBase::DECIMAL) {
  int value = 0;
  for (char chr: str) {
    if (chr != '_')
      value = value * static_cast<int>(base) + char_to_int(chr, base);
  }
  return value;
}

template<typename T>
constexpr std::string int_to_string(T value, IntBase base = IntBase::DECIMAL) {
  std::string str{};
  for (; value != T{}; value /= static_cast<int>(base))
    str += int_to_char(value % static_cast<int>(base), base);
  std::reverse(str.begin(), str.end());
  if (str.empty())
    return "0";
  else
    return str;
}

constexpr bool convertible_to_int_decimal(std::string_view str) {
  return !str.empty() && std::ranges::all_of(str, [](char chr) { return ('0' <= chr) && (chr <= '9'); });
}

constexpr bool is_whitespace(char c) {
  constexpr char whitespaces[]{' ', '\n', '\r', '\f', '\v', '\t'};
  return std::any_of(std::begin(whitespaces), std::end(whitespaces), [c](char chr) { return c == chr; });
}

constexpr std::string escape(const std::string &text) {
  std::string str{"\""};
  for (char chr: text) {
    if (chr == '\n')
      str += "\\n";
    else if (chr == '\r')
      str += "\\r";
    else if (chr == '\t')
      str += "\\t";
    else if (chr == '\v')
      str += "\\v";
    else if (chr == '\a')
      str += "\\a";
    else if (chr == '\f')
      str += "\\f";
    else if (chr == '\"')
      str += "\\\"";
    else if (chr == '\\')
      str += "\\\\";
    else
      str += chr;
  }
  return str + "\"";
}

auto as_string_t(auto str_generator) {
  constexpr std::size_t size = str_generator().size();
  return string_t<size>{str_generator()};
}

template<typename T, auto MaxSize>
struct oversized_array {
  std::array<T, MaxSize> data{};
  std::size_t size{};
};

template<auto MaxSize, typename T>
consteval auto to_oversized_array(const std::vector<T> &bytes) {
  oversized_array<T, MaxSize> result{};
  std::copy(bytes.begin(), bytes.end(), result.data.begin());
  result.size = bytes.size();
  return result;
}

template<auto MaxSize>
consteval auto to_right_sized_array(auto generator) {
  constexpr auto oversized = to_oversized_array<MaxSize>(generator());
  std::array<typename decltype(oversized.data)::value_type, oversized.size> result;
  std::copy(oversized.data.begin(), std::next(oversized.data.begin(), oversized.size), result.begin());
  return result;
}

template<auto MaxSize>
consteval auto to_byte_array(auto generator) {
  return to_right_sized_array<MaxSize>(generator);
}

constexpr auto &take_ref(auto &obj) noexcept {
  return obj;
}

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
  std::atomic_thread_fence(std::memory_order_seq_cst);
  auto res_time = std::chrono::high_resolution_clock::now();
  std::atomic_thread_fence(std::memory_order_seq_cst);
  return res_time;
}

template<class D>
inline long long to_ms(const D &d) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

template<typename T>
constexpr auto get_type_name() -> std::string_view {
#if defined(__clang__)
  constexpr auto prefix = std::string_view{"[T = "};
    constexpr auto suffix = "]";
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
  constexpr auto prefix = std::string_view{"with T = "};
  constexpr auto suffix = "; ";
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
  constexpr auto prefix = std::string_view{"get_type_name<"};
    constexpr auto suffix = ">(void)";
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
# error Unsupported compiler
#endif

  const auto start = function.find(prefix) + prefix.size();
  const auto end = function.find(suffix);
  const auto size = end - start;

  return function.substr(start, size);
}

#endif // AMSL_UTILS_HPP
