#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace plc {

enum class Severity { Note, Warning, Error };

struct LocalPosition {
  std::uint64_t line = 1;
  std::uint64_t column = 1;
};

using fileid_t = std::uint16_t;

struct GlobalPosition {
  LocalPosition local;
  fileid_t file_id;
};

struct Diagnostic {
  Severity severity;
  std::string_view code;
  GlobalPosition span;
  std::string message;
  std::vector<std::string> notes;
  std::optional<std::string> fix;
};

} // namespace plc
