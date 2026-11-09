#pragma once

#include "../../ir/opcode.hpp"
#include <cstdint>

namespace plabi {

using value_t = uint32_t;
using command_t = value_t;
using offset_t = int64_t;
using address_t = uint64_t;
using ip_t = value_t;

struct Command {
  pl::OpCode op;
  union {
    value_t dst;
    ip_t imm;
  };
  value_t a, b;
};

// ! compiler makes a padding length of 16 bytes for decoder conveyor (see
// ! decoder.hpp) ! 13 bytes of OpCode End to forcfully interrupt a programm
inline constexpr std::size_t command_non_required_slots_size =
    3 * sizeof(value_t);

inline constexpr std::size_t command_size = 13;

inline constexpr address_t resolve_command_address(std::size_t idx) noexcept {
  return command_size * idx;
}

} // namespace plabi
