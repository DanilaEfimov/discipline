#pragma once

#include "bounded_hash.hpp"
#include "cache_line.hpp"
#include "cache_line_allocator.hpp"
#include "config.hpp"
#include <algorithm>
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
  using bucket_cnt_t = uint16_t;
  bucket_cnt_t bucket;
  cache_line_size_t count = 0;

  BucketCounter(bucket_cnt_t bucket) : bucket() {};
};

template <class K, class T, class Hash = BoundedHash<K>,
          class KeyEqual = std::equal_to<K>,
          class Allocator = CacheLineAllocator<T>>
class HashMap {
  std::vector<BucketCounter> bucket_counters;
  std::vector<CacheLine, CacheLineAllocator<T>> nodes;
  Hash hasher;

  void pop_bucket(BucketCounter::bucket_cnt_t idx) {}

  bool bucket_contains(const K &key,
                       BucketCounter::bucket_cnt_t bucket) const noexcept {
    size_t items = this->bucket_counters.at(bucket).count;
    const CacheLine &line = this->nodes.at(bucket);
    for (cache_line_size_t i = 0; i < items; ++i) {
      const Node<K, T> *node =
          reinterpret_cast<Node<K, T> *>(&line + i * sizeof(Node<K, T>));
      if (node->value.first == key)
        return true;
    }
    return false;
  }

public:
  HashMap() : bucket_counters(), nodes(), hasher() {
    this->bucket_counters.reserve(cache_line_size);
  };
  ~HashMap() = default;

  void insert(const K &key, const T &value) {
    // ! this function sucks
    cache_line_size_t bucket = this->hasher.calculate(key);
    int chunks_count = this->hasher.chunks_count();

    if (!this->contains(key))
      this->bucket_counters.emplace_back(BucketCounter(bucket));

    // TODO: check the bucket capacity
    // TODO: insert
  }

  void erase(const K &key) {}

  T &operator[](const K &key) noexcept {
    // TODO: searching
  }

  const T &at(const K &key) const {
    if (!this->contains(key))
      throw std::out_of_range("hash_map::HashMap<...>: key is out of range");

    return (*this)[key];
  }

  bool contains(const K &key) const noexcept {
    cache_line_size_t offset = this->hasher.calculate_offset(key);

    for(size_t i = 0; i < this->nodes.size() / cache_line_size; ++i) {
      if(this->bucket_contains(key, i*cache_line_size + offset))
        return true;
    }

    return false;
  }

  bool empty() const noexcept { return this->bucket_counters.empty(); }

  size_t size() const noexcept {
    size_t res = 0;
    for (const auto &counter : this->bucket_counters)
      res += counter.count;
    return res;
  }

  const Hash &get_hasher() const noexcept { return this->hasher; }

  void clear() {
    this->bucket_counters.clear();
    this->nodes.clear();
  }

  size_t shrint_to_fit() {
    // TODO: free not used buckets
    //  @returns: new count of buckets
    return this->bucket_counters.size();
  }
};

} // namespace hash_map
