#pragma once

#include "bounded_hash.hpp"
#include "cache_line.hpp"
#include "cache_line_allocator.hpp"
#include "config.hpp"

#include <algorithm>
#include <functional>
#include <stdint.h>
#include <vector>
#include <optional>
#include <span>

namespace hash_map {

template <class K, class T> class Node {

  using value_t = std::pair<K, T>;

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

  BucketCounter(bucket_cnt_t bucket) : bucket(bucket) {};
  BucketCounter(bucket_cnt_t bucket, cache_line_size_t count)
    : bucket(bucket), count(count) {};

  bool is_full() const noexcept {
    return this->count == cache_line_size;
  }

  bool empty() const noexcept {
    return !(this->count);
  }
};

template <class K, class T, class Hash = BoundedHash<K>,
          class KeyEqual = std::equal_to<K>,
          class Allocator = CacheLineAllocator<T>>
class HashMap {
  using node_t = Node<K, T>;

  std::vector<BucketCounter> bucket_counters;
  std::vector<CacheLine, CacheLineAllocator<T>> nodes;
  Hash hasher;

  void pop_bucket() {
    if(this->empty()) {
      this->nodes.clear();
      return;
    }

    this->hasher.pop_chunk();

    size_t current_size = this->bucket_counters.size();
    this->bucket_counters.resize(current_size - cache_line_size);

    current_size = this->nodes.size();
    this->nodes.resize(current_size - cache_line_size);
  }

  void push_bucket() {
    this->hasher.push_chunk();

    size_t current_size = this->bucket_counters.size();
    this->bucket_counters.resize(current_size + cache_line_size);

    current_size = this->nodes.size();
    this->nodes.resize(current_size + cache_line_size);
  }

  bool bucket_contains(const K &key,
                       BucketCounter::bucket_cnt_t idx) const noexcept {
    const auto& descriptor = this->bucket_counters.at(idx);
    const auto& bucket = this->nodes.at(descriptor.bucket);
    
    const node_t* base = bucket.data();

    for(size_t i = 0; i < descriptor.count; ++i) {
      const node_t* current = base + i;
      if(current->value.first == key)
        return true;
    }

    return false;
  }

  void erase_in_bucket(const K& key, size_t idx) {
    auto& descriptor = this->bucket_counters.at(idx);
    CacheLine& bucket = this->nodes.at(idx);

    if(descriptor.empty())
      return;

    auto* begin = reinterpret_cast<node_t*>(bucket.bytes);
    auto* end = begin + cache_line_size / sizeof(node_t);

    auto* new_end = std::remove_if(begin, end, 
      [key](const node_t& node){ return key == node.value.first; });

    descriptor.count = new_end - begin;
  }

public:
  HashMap() : bucket_counters(), nodes(), hasher() {
    this->bucket_counters.reserve(cache_line_size);
  };
  ~HashMap() = default;

  void insert(const K &key, const T &value) {
    size_t idx = this->hasher.calculate(key);

    auto& descriptor = this->bucket_counters.at(idx);
    if(descriptor.is_full())
      this->push_bucket();

    idx = this->hasher.calculate(key);
    CacheLine& bucket = this->nodes.at(idx);

    Node<K, T> node = Node<K, T>(key, value);
    bucket.write(node, descriptor.count++);
  }

  void erase(const K &key) {
    size_t base = this->hasher.calculate_offset(key);
    size_t hops = this->bucket_counters.size() / cache_line_size;
    for(size_t i = 0; i < hops; i++) {
      this->erase_in_bucket(key, base + i*cache_line_size);
    }
  }

  T &operator[](const K &key) noexcept {
    if(!this->contains(key)) {
      this->push_bucket();
      return T();
    }

    size_t base = this->hasher.calculate_offset(key);
    size_t hops = this->bucket_counters.size() / cache_line_size;
    for(size_t i = 0; i < hops; i++) {
      size_t idx = base + i*cache_line_size;

      const auto& descriptor = this->bucket_counters.at(idx);
      const CacheLine& bucket = this->nodes.at(idx);
      for(size_t j = 0; j < descriptor.count; j++) {
        const node_t* node = reinterpret_cast<node_t*>(bucket.bytes + j*sizeof(node_t));
        if(node->value.first == key)
          return node->value.second;
      }
    }
    
    return T();
  }

  const T &at(const K &key) const {
    if (!this->contains(key))
      throw std::out_of_range("hash_map::HashMap<...>: key is out of range");

    return (*this)[key];
  }

  bool contains(const K &key) const noexcept {
    cache_line_size_t offset = this->hasher.calculate_offset(key);

    for(size_t i = 0; i < this->bucket_counters.size() / cache_line_size; ++i) {
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
};

} // namespace hash_map
