#pragma once

#include "../tokenizer/token.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace plc {

enum class Severity { Note, Warning, Error };

struct Diagnostic {
  Severity severity;
  std::string_view code;
  GlobalPosition span;
  std::string message;
  std::vector<std::string> notes;
  std::optional<std::string> fix;
};

} // namespace plc
