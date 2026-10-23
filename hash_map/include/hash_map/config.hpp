#pragma once

#include <cstddef>
#include <cstdint>
#include <new>

namespace hash_map {

#ifndef HASH_MAP_CACHE_LINE_SIZE
#define HASH_MAP_CACHE_LINE_SIZE 64
#endif

inline constexpr std::size_t cache_line_size = HASH_MAP_CACHE_LINE_SIZE;

template <std::size_t L> struct cache_line_size_trait {
  using v = std::uint16_t;
};

template <> struct cache_line_size_trait<64> {
  using v = std::uint8_t;
};

template <> struct cache_line_size_trait<128> {
  using v = std::uint8_t;
};

using cache_line_size_t = cache_line_size_trait<cache_line_size>::v;

} // namespace hash_map
