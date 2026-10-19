#pragma once

#include "config.hpp"

namespace hash_map {

template<class T>
struct KeyCounter {
    T key;
    cache_line_size_t count = 0;
};

template<
    class T,
    class Key, 
    class Hash,
    class KeyEqual,
    class Allocator
> class HashMap {
    // TODO: SoA cache friendly hash map
};

}   // namespace hash_map

