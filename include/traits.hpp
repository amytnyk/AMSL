#ifndef AMSL_TRAITS_HPP
#define AMSL_TRAITS_HPP

#include <type_traits>
#include <iterator>

template<typename T>
concept Trivial = std::is_trivially_copyable_v<T>;

template<typename T>
concept Vector = requires(T a) {
  typename T::value_type;
  typename T::size_type;
  typename T::iterator;
  typename T::const_iterator;
  { a.begin() } -> std::same_as<typename T::iterator>;
  { a.end() } -> std::same_as<typename T::iterator>;
  { a.cbegin() } -> std::same_as<typename T::const_iterator>;
  { a.cend() } -> std::same_as<typename T::const_iterator>;
  { a.size() } -> std::same_as<typename T::size_type>;
  requires std::forward_iterator<typename T::iterator>;
  requires std::forward_iterator<typename T::const_iterator>;
};

template<typename T>
struct ref_wrapper {
  using type = typename std::conditional<std::is_pointer<T>::value, T, T &>::type;
};

template<typename T>
struct const_ref_wrapper {
  using type = const typename ref_wrapper<T>::type;
};

template<typename T>
using ref_wrapper_t = ref_wrapper<T>::type;

template<typename T>
using const_ref_wrapper_t = const_ref_wrapper<T>::type;

#endif // AMSL_TRAITS_HPP
