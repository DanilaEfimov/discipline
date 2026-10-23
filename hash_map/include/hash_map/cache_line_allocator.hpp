#pragma once

#include "config.hpp"

namespace hash_map {

template <class T> class CacheLineAllocator {

public:
  using value_type = T;

  CacheLineAllocator() = default;

  T *allocate(size_t n) {
    return reinterpret_cast<T *>(
        ::operator new(n * sizeof(T), std::align_val_t{alignof(T)}));
  }

  void deallocate(T *p, size_t) noexcept {
    ::operator delete(p, std::align_val_t{alignof(T)});
  }

  constexpr bool operator==(const CacheLineAllocator &other) const noexcept {
    return true;
  }

  constexpr bool operator!=(const CacheLineAllocator &other) const noexcept {
    return !(*this == other);
  }
};

} // namespace hash_map
