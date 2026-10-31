#pragma once

#include "semantics.hpp"

namespace plp {

template<StatementType T>
class Statement;

template<>
class Statement<StatementType::LabelDeclaration> {

};

template<>
class Statement<StatementType::Operation> {

};

} // namespace plp
