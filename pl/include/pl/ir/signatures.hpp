#pragma once

#include "opcode.hpp"
#include <array>
#include <cstddef>

namespace pl {

using value_t = uint32_t;

enum class ValueCategory { Literal, Variable };

struct OpSignature {
    std::size_t argc;
    std::array<ValueCategory, 2> args;
};

inline constexpr OpSignature op_signature(OpCode op) {
    using VC = ValueCategory;
    switch (op) {
        case OpCode::AddLiteral:   return { 2, { VC::Literal,  VC::Literal  } };
        case OpCode::AddVar:       return { 2, { VC::Literal,  VC::Variable } };
        case OpCode::MulLiteral:   return { 2, { VC::Literal,  VC::Literal  } };
        case OpCode::MulVar:       return { 2, { VC::Literal,  VC::Variable } };
        case OpCode::DivLiteral:   return { 2, { VC::Literal,  VC::Literal  } };
        case OpCode::DivVar:       return { 2, { VC::Literal,  VC::Variable } };
        case OpCode::LeftLiteral:  return { 1, { VC::Literal } };
        case OpCode::LeftVar:      return { 1, { VC::Variable } };
        case OpCode::RightLiteral: return { 1, { VC::Literal  } };
        case OpCode::RightVar:     return { 1, { VC::Variable } };
        case OpCode::WriteLiteral: return { 2, { VC::Literal,  VC::Variable } };
        case OpCode::WriteVar:     return { 2, { VC::Variable,  VC::Variable } };
        case OpCode::Print:        return { 1, { VC::Literal  } };
        case OpCode::Read:         return { 1, { VC::Literal  } };
        case OpCode::JumpLiteral:  return { 1, { VC::Literal } };
        case OpCode::JumpVar:      return { 1, { VC::Variable } };
        case OpCode::End:          return { 0, {} };
    }
    return { 0, {} };
}

} // namespace pl
