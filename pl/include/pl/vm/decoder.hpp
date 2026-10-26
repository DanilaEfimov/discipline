#pragma once

#include "../ir/opcode.hpp"
#include "arithmetic.hpp"
#include "executor.hpp"
#include "mmu.hpp"
#include "workspace.hpp"
#include <cstring>


namespace pl {

// ! [opcode] [dest | imm] [a] [b]
// ! ^^^ abi format offsets vvv
inline constexpr std::size_t opcode_offset = 0;
inline constexpr std::size_t dest_offset = sizeof(OpCode) + opcode_offset;
inline constexpr std::size_t imm_offset = dest_offset;
inline constexpr std::size_t a_offset = sizeof(value_t) + dest_offset;
inline constexpr std::size_t b_offset = sizeof(value_t) + a_offset;
inline constexpr std::size_t command_size = b_offset + sizeof(value_t);


struct Decoder {

  static constexpr bool is_jump(OpCode op) noexcept {
    return op == OpCode::JumpLiteral || op == OpCode::JumpVar || op == OpCode::JumpIf;
  }

  static Command next(const Session &session) {
    const auto* bytes = session.module.data.bytes;

    Command cmd;
    ip_t ip = session.context.ip;

    const auto* base = bytes + ip;
    memcpy(&cmd.op, base + opcode_offset, sizeof(OpCode));

    if(Decoder::is_jump(cmd.op))
      memcpy(&cmd.imm, base + imm_offset, sizeof(ip_t));
    else
      memcpy(&cmd.dst, base + dest_offset, sizeof(value_t));
    
    memcpy(&cmd.a, base + a_offset, sizeof(value_t));
    memcpy(&cmd.b, base + b_offset, sizeof(value_t));

    return cmd;
  }
};

} // namespace pl
