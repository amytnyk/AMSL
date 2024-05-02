#ifndef AMSL_AMSL_HPP
#define AMSL_AMSL_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <variant>
#include "string.hpp"

using namespace std::literals;

#define AMSL_INLINE inline __attribute__((always_inline))

class AMSL {
public:
  template<string_t source_code>
  AMSL_INLINE void execute() {
    std::cout << "Executing: " << source_code.c_str() << std::endl;
  }
};

#endif // AMSL_AMSL_HPP
