#pragma once

#include "../ir/signatures.hpp"
#include <cstring>

namespace pl {

struct MemoryManager {

  static void write_literal(value_t value, std::uint8_t *bytes,
                            std::size_t field) {
    memcpy(bytes + field, &value, sizeof(value_t));
  }

  static void write_literals(std::uint8_t *source, std::uint8_t *bytes,
                             std::size_t field, std::size_t n) {
    memcpy(bytes + field, source, n);
  }

  static value_t read_literal(const std::uint8_t *bytes, std::size_t field) {
    value_t v;
    memcpy(&v, bytes + field, sizeof(value_t));
    return v;
  }
};

} // namespace pl
