#pragma once

#include "../parser/ast.hpp"
#include "abi.hpp"
#include <ostream>
#include <bit>
#include <cstdint>

namespace pla {

template <typename T>
constexpr T to_little_endian(T value) noexcept {
    if constexpr (std::endian::native == std::endian::little) {
        return value;
    } else {
        return std::byteswap(value);
    }
}

class PlAssembler {
  std::ostream &out;

public:
  PlAssembler(std::ostream &o) : out(o) {}

  void write(plp::Operation op) {
    pl::OpCode code = to_little_endian(op.op);
    out.write(reinterpret_cast<const char*>(&code), sizeof(op.op));
    plabi::value_t value = to_little_endian(op.arg1.first);
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    value = to_little_endian(op.arg2.first);
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));
  }

};

} // namespace pla
