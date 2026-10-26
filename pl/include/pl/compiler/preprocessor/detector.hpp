#pragma once

#include <cstdint>

namespace plc {

enum class Trigger {
    Comment,
    Macro,
};

struct LocalPosition {
    std::uint64_t line;
    std::uint64_t column;
};

struct GlobalPosition {
    LocalPosition local;
    const char* filename;
};

inline constexpr const char* 

} // namespace plc
