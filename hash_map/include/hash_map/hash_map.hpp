#pragma once

#include "bounded_hash.hpp"
#include "cache_line.hpp"
#include "cache_line_allocator.hpp"
#include "config.hpp"

#include <algorithm>
#include <functional>
#include <optional>
#include <span>
#include <stdint.h>
#include <vector>

namespace hash_map {

template <class K, class T> struct Node {
  K key;
  T value;

  Node(const K &key, const T &val) : key(key), value(val) {}
};

struct BucketCounter {
  using bucket_cnt_t = size_t;
  cache_line_size_t count = 0;

  BucketCounter() = default;
  BucketCounter(cache_line_size_t count) : count(count) {};

  bool is_full(size_t capacity) const noexcept {
    return this->count >= capacity;
  }

  bool empty() const noexcept { return !(this->count); }
};

template <class K, class T, class Hash = BoundedHash<K>,
          class KeyEqual = std::equal_to<K>,
          class Allocator = CacheLineAllocator<CacheLine>>
class HashMap {
  using node_t = Node<K, T>;

  std::vector<BucketCounter> bucket_counters;
  std::vector<CacheLine, Allocator> nodes;
  Hash hasher;

  void pop_bucket() {
    if (this->empty()) {
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
    const auto &descriptor = this->bucket_counters.at(idx);
    const auto &bucket = this->nodes.at(idx);

    const node_t *base = reinterpret_cast<const node_t *>(bucket.bytes);

    for (size_t i = 0; i < descriptor.count; ++i) {
      const node_t *current = base + i;
      if (current->key == key)
        return true;
    }

    return false;
  }

  void erase_in_bucket(const K &key, size_t idx) {
    auto &descriptor = this->bucket_counters.at(idx);
    CacheLine &bucket = this->nodes.at(idx);

    if (descriptor.empty())
      return;

    auto *begin = reinterpret_cast<node_t *>(bucket.bytes);
    auto *end = begin + descriptor.count;

    auto *new_end = std::remove_if(
        begin, end, [key](const node_t &node) { return key == node.key; });

    descriptor.count = new_end - begin;
  }

  const T &access_helper(const K &key) const {
    size_t base = this->hasher.calculate_offset(key);
    size_t hops = this->bucket_counters.size() / cache_line_size;
    for (size_t i = 0; i < hops; i++) {
      size_t idx = base + i * cache_line_size;

      const auto &descriptor = this->bucket_counters.at(idx);
      const CacheLine &bucket = this->nodes.at(idx);
      for (size_t j = 0; j < descriptor.count; j++) {
        const node_t *node =
            reinterpret_cast<const node_t *>(bucket.bytes + j * sizeof(node_t));
        if (node->key == key)
          return node->value;
      }
    }

    // ! this branch has to be unreachable
    throw std::runtime_error("HashMap<...>::access_helper: non-resolved call");
  }

  T &access_helper(const K &key) {
    size_t base = this->hasher.calculate_offset(key);
    size_t hops = this->bucket_counters.size() / cache_line_size;
    for (size_t i = 0; i < hops; i++) {
      size_t idx = base + i * cache_line_size;

      const auto &descriptor = this->bucket_counters.at(idx);
      CacheLine &bucket = this->nodes.at(idx);
      for (size_t j = 0; j < descriptor.count; j++) {
        node_t *node =
            reinterpret_cast<node_t *>(bucket.bytes + j * sizeof(node_t));
        if (node->key == key)
          return node->value;
      }
    }

    // ! this branch has to be unreachable
    throw std::runtime_error("HashMap<...>::access_helper: non-resolved call");
  }

public:
  static constexpr size_t node_capacity = cache_line_size / sizeof(node_t);
  static_assert(node_capacity >= 1,
                "HashMap<...>: node is larger than cahce line");

  HashMap()
      : bucket_counters(cache_line_size), nodes(cache_line_size), hasher() {};
  ~HashMap() = default;

  void insert(const K &key, const T &value) {
    size_t idx = this->hasher.calculate(key);

    if (this->bucket_counters.at(idx).is_full(node_capacity)) {
      this->push_bucket();
      idx = this->hasher.calculate(key);
    }

    idx = this->hasher.calculate(key);
    auto &descriptor = this->bucket_counters.at(idx);
    this->nodes.at(idx).write(node_t(key, value),
                              sizeof(node_t) * descriptor.count);
    ++descriptor.count;
  }

  void erase(const K &key) {
    size_t base = this->hasher.calculate_offset(key);
    size_t hops = this->bucket_counters.size() / cache_line_size;
    for (size_t i = 0; i < hops; i++) {
      this->erase_in_bucket(key, base + i * cache_line_size);
    }
  }

  T &operator[](const K &key) {
    if (!this->contains(key)) {
      this->insert(key, {});
    }
    return this->access_helper(key);
  }

  const T &at(const K &key) const {
    if (!this->contains(key))
      throw std::out_of_range("hash_map::HashMap<...>: key is out of range");
    return this->access_helper(key);
  }

  bool contains(const K &key) const noexcept {
    cache_line_size_t offset = this->hasher.calculate_offset(key);

    for (size_t i = 0; i < this->bucket_counters.size() / cache_line_size;
         ++i) {
      if (this->bucket_contains(key, i * cache_line_size + offset))
        return true;
    }

    return false;
  }

  bool empty() const noexcept { return this->size() == 0; }

  size_t size() const noexcept {
    size_t res = 0;
    for (const auto &counter : this->bucket_counters)
      res += counter.count;
    return res;
  }

  const Hash &get_hasher() const noexcept { return this->hasher; }

  void clear() {
    this->bucket_counters.assign(cache_line_size, BucketCounter{});
    this->nodes.assign(cache_line_size, CacheLine{});
    this->hasher = Hash{};
  }
};

} // namespace hash_map
