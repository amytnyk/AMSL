#ifndef AMSL_PTR_WRAPPER_HPP
#define AMSL_PTR_WRAPPER_HPP

#include <algorithm>

template<typename T>
class ptr_wrapper {
public:
  constexpr ptr_wrapper() = default;

  constexpr ptr_wrapper(const ptr_wrapper<T> &) = delete;

  constexpr ptr_wrapper(ptr_wrapper<T> &&other) noexcept {
    swap(other);
  }

  template<typename U>
  constexpr ptr_wrapper(ptr_wrapper<U> &&other) noexcept : // NOLINT(*-explicit-constructor)
    ptr_wrapper{static_cast<T *>(other.data)} {
    other.data = nullptr;
  }

  constexpr ptr_wrapper(std::nullptr_t) {} // NOLINT(*-explicit-constructor)

  constexpr explicit ptr_wrapper(T *ptr) : data{ptr} {}

  constexpr ptr_wrapper<T> &operator=(const ptr_wrapper<T> &) = delete;

  constexpr ptr_wrapper<T> &operator=(ptr_wrapper<T> &&other) noexcept {
    swap(other);
    return *this;
  }

  template<typename U>
  constexpr ptr_wrapper<T> &operator=(ptr_wrapper<U> &&other) noexcept {
    ptr_wrapper<T> wrapper{std::move(other)};
    swap(wrapper);
    return *this;
  }

  constexpr void swap(ptr_wrapper<T> &other) {
    std::swap(data, other.data);
  }

  constexpr T &operator*() const {
    return *data;
  }

  constexpr T *operator->() const {
    return data;
  }

  constexpr T *get() const {
    return data;
  }

  constexpr explicit operator bool() const {
    return data != nullptr;
  }

  constexpr ~ptr_wrapper() {
    delete data;
  }

  template<typename U> friend
  class ptr_wrapper;

private:
  T *data{};
};

template<typename T, typename ... Args>
constexpr ptr_wrapper<T> make_ptr_wrapper(Args &&... args) {
  return ptr_wrapper{new T{std::forward<Args>(args)...}};
}

#endif // AMSL_PTR_WRAPPER_HPP
