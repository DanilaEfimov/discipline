#pragma once

#include "../ir/opcode.hpp"
#include "arithmetic.hpp"
#include "exceptions.hpp"
#include "mmu.hpp"
#include "workspace.hpp"
#include <string>


namespace pl {

using command_t = value_t;
using offset_t = int64_t;
using address_t = uint64_t;

struct Command {
  OpCode op;
  value_t dst, a, b;
  ip_t imm;
};

struct Executor {

  MemoryManager mmu;
  Arithmetic calculator;
  Workspace &workspace;

  Executor(Workspace &workspace) : mmu(), calculator(), workspace(workspace) {}

  bool is_value_address_out_of_range(const Session &session,
                                     address_t address) noexcept {
    offset_t offset = address + sizeof(value_t);
    return this->is_offset_out_of_range(session, offset);
  }

  bool is_offset_out_of_range(const Session &session,
                              offset_t offset) noexcept {
    const std::uint64_t &current_ip = session.context.ip;
    std::uint64_t length = session.module.data.header.length;

    if (offset >= 0) {
      return (offset >= length - current_ip || current_ip >= length);
    } else {
      const std::uint64_t abs_offset =
          static_cast<std::uint64_t>(-(offset + 1)) + 1;
      return abs_offset > current_ip;
    }

    return false;
  }

  void shift_ip(offset_t offset) {
    auto &current_session = this->workspace.get_current_session();
    std::uint64_t &current_ip = current_session.context.ip;
    std::uint64_t length = current_session.module.data.header.length;

    if (this->is_offset_out_of_range(current_session, offset))
      throw PlSegmentFault("instruction pointer is out of range");

    if (offset >= 0)
      current_ip += static_cast<std::uint64_t>(offset);
    else {
      const std::uint64_t abs_offset =
          static_cast<std::uint64_t>(-(offset + 1) + 1);
      current_ip -= abs_offset;
    }
  }

  void set_ip(ip_t new_ip) noexcept {
    auto &current_context = this->workspace.get_current_session().context;
    current_context.ip = new_ip;
  }

  void shirt_ip_right(offset_t offset) { this->shift_ip(offset); }

  void shift_ip_left(offset_t offset) { this->shift_ip(-offset); }

  void set_field(address_t address, value_t value) {
    auto &current_session = this->workspace.get_current_session();
    auto &current_data = current_session.module.data.bytes;

    if (this->is_value_address_out_of_range(current_session, address))
      throw PlSegmentFault("value address is out of range");

    this->mmu.write_literal(value, current_data, address);
  }

  value_t get_field(address_t address) {
    auto &current_session = this->workspace.get_current_session();
    auto &current_data = current_session.module.data.bytes;

    if (this->is_value_address_out_of_range(current_session, address))
      throw PlSegmentFault("value address is out of range");

    return this->mmu.read_literal(current_data, address);
  }

  void output(char c) { this->workspace.get_output_stream() << c; }

  void switch_session(std::size_t session_idx) {
    this->workspace.switch_session(session_idx);
  }
  void run_command(Command &command) {
    auto &current_session = this->workspace.get_current_session();
    auto *bytes = current_session.module.data.bytes;

    switch (command.op) {
    case OpCode::End:
      return;
    case OpCode::AddLiteral: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = command.a;
      value_t result = this->calculator.add_literals(left, right);
      this->mmu.write_literal(result, bytes,
                              command.dst);
    } break;
    case OpCode::AddVar: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = this->mmu.read_literal(bytes, command.a);
      value_t result = this->calculator.add_literals(left, right);
      this->mmu.write_literal(result, bytes,
                              command.dst);
    } break;
    case OpCode::MulLiteral: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = command.a;
      value_t result = this->calculator.mul_literals(left, right);
      this->mmu.write_literal(result, bytes,
                              command.dst);
    } break;
    case OpCode::MulVar: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = this->mmu.read_literal(bytes, command.a);
      value_t result = this->calculator.mul_literals(left, right);
      this->mmu.write_literal(result, bytes,
                              command.dst);
    } break;
    case OpCode::DivLiteral: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = command.a;
      value_t result = this->calculator.div_literals(left, right);
      this->mmu.write_literal(result, bytes,
                              command.dst);
    } break;
    case OpCode::DivVar: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = this->mmu.read_literal(bytes, command.a);
      value_t result = this->calculator.div_literals(left, right);
      this->mmu.write_literal(result, bytes,
                              command.dst);
    } break;
    case OpCode::EqualLiteral: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = command.a;
      value_t result = this->calculator.eq_literals(left, right);
      this->mmu.write_literal(result, bytes, command.dst);
    } break;
    case OpCode::EqualVar: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = this->mmu.read_literal(bytes, command.a);
      value_t result = this->calculator.eq_literals(left, right);
      this->mmu.write_literal(result, bytes, command.dst);
    } break;
    case OpCode::LessLiteral: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = command.a;
      value_t result = this->calculator.le_literals(left, right);
      this->mmu.write_literal(result, bytes, command.dst);
    } break;
    case OpCode::LessVar: {
      value_t left = this->mmu.read_literal(bytes, command.dst);
      value_t right = this->mmu.read_literal(bytes, command.a);
      value_t result = this->calculator.le_literals(left, right);
      this->mmu.write_literal(result, bytes, command.dst);
    } break;
    case OpCode::RightLiteral: {
      value_t shift = command.a;
      this->shirt_ip_right(shift);
    } break;
    case OpCode::RightVar: {
      value_t shift = this->mmu.read_literal(bytes, command.dst);
      this->shirt_ip_right(shift);
    } break;
    case OpCode::LeftLiteral: {
      value_t shift = command.a;
      this->shift_ip_left(shift);
    } break;
    case OpCode::LeftVar: {
      value_t shift = this->mmu.read_literal(bytes, command.dst);
      this->shift_ip_left(shift);
    } break;
    case OpCode::WriteLiteral: {
      value_t dest = command.dst;
      value_t value = command.a;
      this->mmu.write_literal(value, bytes, dest);
    } break;
    case OpCode::WriteVar: {
      value_t dest = this->mmu.read_literal(bytes, command.a);
      value_t value = command.a;
      this->mmu.write_literal(value, bytes, dest);
    } break;
    case OpCode::Print: {
      value_t value = this->mmu.read_literal(bytes, command.dst);
      char c = static_cast<char>(value & 0xFF);
      this->output(c);
    } break;
    case OpCode::Read: {
      value_t dest = command.dst;

      std::string line;
      std::getline(this->workspace.get_input_stream(), line);

      std::size_t length = current_session.module.data.header.length;
      if (line.size() > length - dest)
        throw PlRuntimeError("the input is too large to fit in memory");

      this->mmu.write_literals(reinterpret_cast<std::uint8_t *>(line.data()),
                               bytes, dest, line.size());
    } break;
    case OpCode::JumpLiteral: {
      value_t dest = command.dst;
      this->set_ip(dest);
    } break;
    case OpCode::JumpVar: {
      value_t dest = this->mmu.read_literal(command.a);
      this->set_ip(dest);
    } break;
    default: {
      std::string error = "no such command named: " +
                          std::to_string(static_cast<std::size_t>(command.op));
      throw PlUndefinedCommand(error);
    }
    }
  }
};

} // namespace pl
