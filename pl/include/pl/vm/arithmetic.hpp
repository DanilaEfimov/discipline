#pragma once

#include "../ir/signatures.hpp"
#include "exceptions.hpp"
#include <string>

namespace pl {

struct Arithmetic {
  static value_t add_literals(value_t left, value_t right) noexcept {
    return left + right;
  }

  static value_t mul_literals(value_t left, value_t right) noexcept {
    return left * right;
  }

  static value_t div_literals(value_t left, value_t right) {
    if (right == 0) {
      std::string error =
          std::string("division by zero by address: ") + std::to_string(left);
      throw PlDivisionByZero(error);
    }
    return left / right;
  }

  static value_t eq_literals(value_t left, value_t right) noexcept {
    return left == right ? 1 : 0;
  }

  static value_t le_literals(value_t left, value_t right) noexcept {
    return left < right;
  }
};

} // namespace pl
