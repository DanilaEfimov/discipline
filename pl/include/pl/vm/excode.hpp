#pragma once

#include <cstdint>

namespace pl {

enum class ExCode : std::uint32_t {
    Success = 0,
    UndefinedCommand,
    SegmentFault,
    RuntimeError,
};

} // namespace pl
