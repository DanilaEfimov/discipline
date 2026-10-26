#pragma once

#include <concepts>
#include <cstdint>
#include <format>
#include <optional>
#include <string>

namespace plc {

using diagnostics_code_t = std::uint32_t;

struct DiagnosticsCode {
  diagnostics_code_t c;
};

struct CompileMessage {
  const char *message;
  std::optional<DiagnosticsCode> code;
  std::optional<const char *> details;
};

// ! this function sucks without hex-formatted code vvv
template <class T>
  requires std::is_base_of<CompileMessage, T>
inline std::string format_message(T msg) {
  std::string formatted = msg.message;
  if (msg.code.has_value())
    formatted += std::string(" (") + std::to_string(msg.code.value().c) + ")";
  formatted += "\n";
  if (msg.details.has_value())
    formatted += std::string("details: ") + msg.details.value();
  return formatted;
};

} // namespace plc
