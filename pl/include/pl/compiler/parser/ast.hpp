#pragma once

#include "../../ir/opcode.hpp"
#include "../../ir/signatures.hpp"
#include "../assembler/abi.hpp"
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace plp {

enum class StatementType {
  Declaration,
  Operation,
  None,
};

struct Declaration {
  std::string identifier;
  std::size_t address;
};

using arg_t = std::pair<plabi::value_t, pl::ValueCategory>;

struct Operation {
  arg_t arg1;
  arg_t arg2;
  pl::OpCode op;
};

// ! order has to match the StatementType members order
using statement_t = std::variant<Declaration, Operation>;
using program_t = std::vector<Operation>;

inline pl::OpSignature get_op_signature(const Operation &operation) {
  pl::OpSignature sign;

  sign.argc += operation.arg1.second == pl::ValueCategory::None ? 0 : 1;
  sign.argc += operation.arg2.second == pl::ValueCategory::None ? 0 : 1;

  sign.args[0] = operation.arg1.second;
  sign.args[0] = operation.arg2.second;

  return sign;
}

inline bool signature_match(const Operation &operation) {
  return get_op_signature(operation) == pl::op_signature(operation.op);
}

} // namespace plp
