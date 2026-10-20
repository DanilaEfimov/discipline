#pragma once

#include "bounded_hash.hpp"
#include "cache_line.hpp"
#include "cache_line_allocator.hpp"
#include "config.hpp"
#include <functional>
#include <stdint.h>
#include <vector>

namespace hash_map {

template <class K, class T> class Node {

  using value_t = std::pair<const K, T>;

  static constexpr size_t pad =
      (cache_line_size - sizeof(value_t) % cache_line_size) % cache_line_size;

  value_t value;
  char padding[pad];

  static_assert(
      (sizeof(value_t) + pad) % cache_line_size == 0,
      "hash_map::Node<K,T>: data node size is not aligned by cache line size");
};

struct BucketCounter {
  uint16_t bucket;
  cache_line_size_t count = 0;
};

template <class K, class T, class Hash = BoundedHash<K>,
          class KeyEqual = std::equal_to<K>,
          class Allocator = CacheLineAllocator>
class HashMap {
  // TODO: SoA cache friendly hash map

  std::vector<BucketCounter> buckets;
  std::vector<CacheLine, CacheLineAllocator> nodes;
  Hash hasher;

  // TODO: easy allocation api

public:
  HashMap() : buckets(), nodes(), hasher() {};
  ~HashMap() = default;

  void insert(const K &key, const T &value) {
    // TODO: insert a new Node<K, T> into a cache line
  }

  void erase(const K &key) {}

  const T &operator[](const K &key) const noexcept {
    // TODO: searching
    return {};
  }

  T &operator[](const K &key) noexcept {
    // TODO: searching
  }

  const T &at(const K &key) const {
    if (!this->contains(key))
      throw std::out_of_range("");

    return (*this)[key];
  }

  bool contains(const K &key) const noexcept {
    // TODO: bucket bypassing
    return false;
  }

  bool empty() const noexcept { return this->buckets.empty(); }

  size_t size() const noexcept {
    // TODO: count elements per bucket
    return 0;
  }

  const Hash &get_hasher() const noexcept { return this->hasher; }

  void clear() {
    // TODO: cleaning
  }

  size_t shrint_to_fit() {
    // TODO: free not used buckets
    //  @returns: new count of buckets
    return this->buckets.size();
  }
};

} // namespace hash_map
