#pragma once

#include "config.hpp"
#include <functional>

namespace hash_map {

template<class T>
class BoundedHash {
    using chunks_t = unsigned int;
    chunks_t chunks = 0;

public:
    cache_line_size_t calculate(T&& obj) {
        return cache_line_size * this->chunks + 
            std::hash<T>(obj) & (cache_line_size - 1);
    }

    void push_chunk() noexcept {
        this->chunks++;
    }

    void pop_chunk() noexcept {
        this->chunks--;
    }

    chunks_t chunks_count() noexcept {
        return this->chunks;
    }

};

}   // namespace hash_map

