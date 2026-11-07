#pragma once

#define ERROR_CODE inline constexpr const char *

namespace plc {

// section E0* vvv
ERROR_CODE undefined_symbol = "E02";
ERROR_CODE redefinition = "E03";

// section E1* vvv
ERROR_CODE expected_token = "E10";
ERROR_CODE unexpected_token = "E11";
ERROR_CODE too_many_operands = "E12";
ERROR_CODE unexpected_end = "E13";
ERROR_CODE empty_expression = "E14";

} // namespace plc
