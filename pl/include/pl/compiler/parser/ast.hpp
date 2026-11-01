#pragma once

#include "../assembler/abi.hpp"
#include "../diagnostics/diagnostic.hpp"
#include "../keywords.hpp"
#include <variant>
#include <vector>

namespace plp {

enum class ExpressionType {
  Identifier, // identifier
  Literal,    // literal
  Address,    // '[' literal ']'
};

struct Expression {
  plc::GlobalPosition span;
  ExpressionType type;
  std::string identifier;
  plabi::value_t value;
};

struct Operation {
  plc::GlobalPosition span;
  plc::keyword_id_t keyword_id;
  std::optional<Expression> arg1;
  std::optional<Expression> arg2;
};

inline constexpr std::size_t max_operation_length =
    7; // keyword [expression [expression]]

struct Declaration {
  plc::GlobalPosition span;
  std::string identigier;
  std::optional<plabi::address_t> address;
};

inline constexpr std::size_t max_declaration_length =
    3; // '.' identifier [address]

using Statement = std::variant<Declaration, Operation>;

struct Program {
  std::vector<Statement> statements;
};

} // namespace plp
