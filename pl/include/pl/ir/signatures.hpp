#pragma once

#include "opcode.hpp"
#include <array>
#include <cstddef>

namespace pl {

enum class ValueCategory { None, Literal, Variable };

inline constexpr std::size_t max_arg_count = 2;

struct OpSignature {
  std::size_t argc;
  std::array<ValueCategory, max_arg_count> args;
};

inline bool operator==(const pl::OpSignature &left,
                       const pl::OpSignature &right) {
  if (left.argc != right.argc)
    return false;
  for (int i = 0; i < left.argc; ++i) {
    if (left.args[i] != right.args[i])
      return false;
  }
  return true;
}

inline constexpr OpSignature op_signature(OpCode op) {
  using VC = ValueCategory;
  switch (op) {
  case OpCode::AddLiteral:
    return {2, {VC::Literal, VC::Literal}};
  case OpCode::AddVar:
    return {2, {VC::Literal, VC::Variable}};
  case OpCode::MulLiteral:
    return {2, {VC::Literal, VC::Literal}};
  case OpCode::MulVar:
    return {2, {VC::Literal, VC::Variable}};
  case OpCode::DivLiteral:
    return {2, {VC::Literal, VC::Literal}};
  case OpCode::DivVar:
    return {2, {VC::Literal, VC::Variable}};
  case OpCode::EqualLiteral:
    return {2, {VC::Variable, VC::Literal}};
  case OpCode::EqualVar:
    return {2, {VC::Variable, VC::Variable}};
  case OpCode::LessLiteral:
    return {2, {VC::Variable, VC::Literal}};
  case OpCode::LessVar:
    return {2, {VC::Variable, VC::Variable}};
  case OpCode::LeftLiteral:
    return {1, {VC::Literal}};
  case OpCode::LeftVar:
    return {1, {VC::Variable}};
  case OpCode::RightLiteral:
    return {1, {VC::Literal}};
  case OpCode::RightVar:
    return {1, {VC::Variable}};
  case OpCode::WriteLiteral:
    return {2, {VC::Literal, VC::Variable}};
  case OpCode::WriteVar:
    return {2, {VC::Variable, VC::Variable}};
  case OpCode::Print:
    return {1, {VC::Literal}};
  case OpCode::Read:
    return {1, {VC::Literal}};
  case OpCode::JumpLiteral:
    return {1, {VC::Literal}};
  case OpCode::JumpVar:
    return {1, {VC::Variable}};
  case OpCode::SwitchLiteral:
    return {1, {VC::Literal}};
  case OpCode::SwitchVar:
    return {1, {VC::Variable}};
  case OpCode::JumpIf:
    return {2, {VC::Variable}};
  case OpCode::End:
    return {0, {}};
  }
  return {0, {}};
}

} // namespace pl
