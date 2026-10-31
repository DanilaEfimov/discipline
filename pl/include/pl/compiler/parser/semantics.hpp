#pragma once

namespace plp {

// ! there are few statements:
// 1. label declaration
// * .identifier literal
// 2. operation
// * operation [arg1] [arg2]
enum class StatementType {
    Unknown,
    LabelDeclaration,
    Operation,
};

} // namespace plp
