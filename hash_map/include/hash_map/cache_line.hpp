#pragma once

#include "config.hpp"
#include <stdexcept>
#include <string.h>

namespace hash_map {

struct CacheLine {
  char bytes[cache_line_size];

  template <class T> void write(T &obj, cache_line_size_t dest) {
    static_assert(std::is_trivially_copyable_v<T>,
                  "CacheLine: T has to be trivially copyable");
    static_assert(sizeof(T) <= cache_line_size,
                  "object too large for cache line");

    if (dest > cache_line_size - sizeof(T))
      throw std::out_of_range("hash_map::CacheLine: index out of range");

    memcpy(bytes + dest, std::addressof(obj), sizeof(T));
  }

  template <class T> void read(T &obj, cache_line_size_t src) {
    static_assert(std::is_trivially_copyable_v<T>,
                  "CacheLine: T has to be trivially copyable");
    static_assert(sizeof(T) <= cache_line_size,
                  "object too large for cache line");

    if (src > cache_line_size - sizeof(T))
      throw std::out_of_range("hash_map::CacheLine: index out of range");

    memcpy(std::addressof(obj), bytes + src, sizeof(T));
  }
};

} // namespace hash_map
