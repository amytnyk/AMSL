#ifndef AMSL_ENCODER_HPP
#define AMSL_ENCODER_HPP

#include "bytes.hpp"
#include "traits.hpp"
#include "ptr_wrapper.hpp"

template<typename T>
struct Encoder;

template<typename T>
constexpr void encode(Bytes &bytes, const T &value) {
  return Encoder<T>::encode(bytes, value);
}

template<Trivial T>
struct Encoder<T> {
  static constexpr void encode(Bytes &bytes, const T &value) {
    auto binary_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
    bytes.insert(bytes.end(), binary_representation.begin(), binary_representation.end());
  }
};

template<typename T>
struct Encoder<ptr_wrapper<T>> {
  static constexpr void encode(Bytes &bytes, const ptr_wrapper<T> &value) {
    ::encode(bytes, value.get());
  }
};

template<typename T>
struct Encoder<std::optional<T>> {
  static constexpr void encode(Bytes &bytes, const std::optional<T> &value) {
    auto exists = value.has_value();
    ::encode(bytes, exists);
    if (exists)
      ::encode(bytes, value.value());
  }
};

template<typename T>
struct Encoder<T *> {
  static constexpr void encode(Bytes &bytes, const T *value) {
    if constexpr (std::is_base_of_v<Encodable, T>)
      value->encode(bytes);
    else
      ::encode(bytes, *value);
  }
};

template<Vector T>
struct Encoder<T> {
  static constexpr void encode(Bytes &bytes, const T &value) {
    ::encode(bytes, value.size());
    for (const auto &item: value)
      ::encode(bytes, item);
  }
};

template<typename T>
constexpr Bytes encode_to_bytes(const T &value) {
  Bytes bytes;
  Encoder<T>::encode(bytes, value);
  return bytes;
}

#endif // AMSL_ENCODER_HPP
