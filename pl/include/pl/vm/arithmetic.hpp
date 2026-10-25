#pragma once

#include "../ir/signatures.hpp"

namespace pl {

struct Arithmetic {
  static value_t add_literals(value_t left, value_t right) noexcept {
    return left + right;
  }

  static value_t mul_literals(value_t left, value_t right) noexcept {
    return left * right;
  }

  static value_t div_literals(value_t left, value_t right) {
    return left / right;
  }
};

} // namespace pl
