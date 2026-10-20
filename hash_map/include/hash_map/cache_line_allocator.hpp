#pragma once

#include "config.hpp"

namespace hash_map {

template <class T> class CacheLineAllocator {

public:
  using value_type = T;

  CacheLineAllocator() = default;

  T *allocate(size_t) {
    return reinterpret_cast<T *>(new char[cache_line_size]);
  }

  void deallocate(T *p, size_t) noexcept {
    delete[] reinterpret_cast<char *>(p);
  }

  constexpr bool operator==(const CacheLineAllocator &other) const noexcept {
    return true;
  }

  constexpr bool operator!=(const CacheLineAllocator &other) const noexcept {
    return !(*this == other);
  }
};

} // namespace hash_map
