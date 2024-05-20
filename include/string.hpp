#ifndef AMSL_STRING_HPP
#define AMSL_STRING_HPP

#include <algorithm>
#include <array>
#include <string>
#include <string_view>

template<std::size_t N>
struct string_t {
  static constexpr auto Size = N - 1;
  static constexpr bool Empty = N <= 1;

  std::array<char, N> data{};

  consteval string_t() = default;

  consteval string_t(const char (&str)[N]) { // NOLINT(*-explicit-constructor)
    std::copy_n(str, N, data.begin());
  }

  consteval string_t(const std::string &str) { // NOLINT(*-explicit-constructor)
    std::copy_n(str.begin(), N, data.begin());
  }

  static consteval string_t<N> from_cstr(const char *str) {
    string_t<N> string{};
    std::copy_n(str, N - 1, string.data);
    string.data[N - 1] = '\0';
    return string;
  }

  static consteval string_t<N - 1> from_view(std::string_view sv) { // NOLINT(*-explicit-constructor)
    static_assert(sv.size() + 1 == N, "String view size must match string_t size");
    string_t<N> string{};
    std::copy_n(sv.data(), sv.size(), string.data);
    string.data[N - 1] = '\0';
    return string;
  }

  consteval bool operator==(const string_t<N> &str) const {
    return std::equal(str.data, str.data + N, data);
  }

  template<std::size_t N2>
  consteval bool operator==(const string_t<N2> &) const {
    return false;
  }

  template<std::size_t N2>
  consteval string_t<N + N2 - 1> operator+(const string_t<N2> &str) const {
    char new_data[N + N2 - 1]{};
    std::copy_n(data.begin(), N - 1, new_data);
    std::copy_n(str.data.begin(), N2, new_data + N - 1);
    return new_data;
  }

  consteval char operator[](std::size_t n) const {
    return data[n];
  }

  [[nodiscard]] consteval std::size_t size() const {
    return N - 1;
  }

  [[nodiscard]] consteval std::size_t find_first_of(char c, size_t start = 0) const {
    for (size_t i = start; i < N - 1; ++i)
      if (data[i] == c)
        return i;
    return std::string::npos;
  }

  template<std::size_t Pos, std::size_t Len = N - 1 - Pos>
  [[nodiscard]] consteval string_t<Len + 1> substr() const {
    static_assert(Pos + Len < N, "substr range out of bounds");
    char new_data[Len + 1]{};
    std::copy_n(data.begin() + Pos, Len, new_data);
    new_data[Len] = '\0';
    return string_t<Len + 1>{new_data};
  }

  [[nodiscard]] consteval const char *c_str() const {
    return &data.at(0);
  }

  [[nodiscard]] consteval std::string_view sv() const {
    return std::string_view{data.begin(), data.end()};
  }
};

#endif // AMSL_STRING_HPP
