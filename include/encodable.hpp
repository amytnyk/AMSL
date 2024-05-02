#ifndef AMSL_ENCODABLE_HPP
#define AMSL_ENCODABLE_HPP

#include "bytes.hpp"

struct Encodable {
  virtual constexpr void encode(Bytes &bytes) const = 0;
};

#endif // AMSL_ENCODABLE_HPP
