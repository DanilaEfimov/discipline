#pragma once

#include "config.hpp"
#include "cache_line_allocator.hpp"
#include "bounded_hash.hpp"
#include "cache_line.hpp"
#include <functional>
#include <vector>


namespace hash_map {

template<class T>
struct KeyCounter {
    T key;
    cache_line_size_t count = 0;
};

template<
    class K,
    class T,
    class Hash=BoundedHash<T>,
    class KeyEqual=std::equal_to<K>,
    class Allocator=CacheLineAllocator
> class HashMap {
    // TODO: SoA cache friendly hash map

    using key_t = KeyCounter<K>;

    std::vector<key_t> keys;
    std::vector<CacheLine, CacheLineAllocator> values;

};

}   // namespace hash_map

