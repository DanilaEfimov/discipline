#pragma once

#include <cstddef>
#include <new>

namespace hash_map {

#if defined(__cpp_lib_hardware_interference_size)
inline constexpr std::size_t cache_line_size =
    std::hardware_destructive_interference_size;
#else
inline constexpr std::size_t cache_line_size = 64; // default
#endif

template <std::size_t L> struct cache_line_size_trait {
  using v = short;
};

template <> struct cache_line_size_trait<64> {
  using v = char;
};

template <> struct cache_line_size_trait<128> {
  using v = char;
};

using cache_line_size_t = cache_line_size_trait<cache_line_size>::v;

} // namespace hash_map

