# Hash map

## About project

I was interested in _cmake_, _gtest_ (testing in general). For exploring these tools I have had to start a project. I have picked a Hash Map structure. I'm watching one blogger telling about different benchmarks and optimizations. [He](https://www.youtube.com/@MrTeamlead) uses _Rust_.

## The Point

Mr. Teamlead has telling about SoA (struct of arrays) and cache hits. I was trying to mix SoA and caches in any way. The purpose of the project was an improving code infrastructure skills, so I was free to realize this Hash Map:

1. Template Parameters:
```C++
template <class K, class T, class Hash = BoundedHash<K>,
          class KeyEqual = std::equal_to<K>,
          class Allocator = CacheLineAllocator<CacheLine>>
class HashMap;  // namespace hash_map
```
I was referencing to real `std::unordered_map` implementation, (but in a wrong way)... Don't see the `hash_map::CacheLineAllocator` - I have just tried to make a custom allocator. And don't see the `hash_map::BoundedHash` - it's a special hasher, which follows the contract _"just hash objects to a bounded segment by the uniform distribution"_. You can try make an own hasher btw. But `hash_map::CacheLineAllocator` is the most regular thing - useless, just for fun.
2. Class Members:
```C++
{
  using node_t = Node<K, T>;

  std::vector<BucketCounter> bucket_counters;
  std::vector<CacheLine, Allocator> nodes;
  Hash hasher;
}   // namespace hash_map
```
*bucket_counters* just is a vector of *top* stack-like pointers. It may be just a `std::uint32_t`/`std::uint64_t`, but at the start `BucketCounter` has had store a bucket number, but it's turned out useless.
*nodes* is a vector of buckets (cache lines). Every key-value item stores in a single cache line. It requires for a _false sharing_ resistance (but I guess it's not actually). And these members declared as vectors for _SoA_-like behaviour (but I guess it's not actually).

Note: `template<class K, class T> hash_map::Node<K, T>` at the start was not just a pair of values. It has a padding to be packed in a cache line with other nodes. But it was replaced with a special directive.

## Benchmarks

When I was making the tests I have asked _Claude_ for a benchmark. Obviously, this structure sucks (but there is an analog in _std_ and _Boost_ implemented in a pretty better way). So let's see the results.

### Test environmebt

- **OS**: Arch Linux x86_64 (kernel 7.2)
- **CPU**: AMD Ryzen 7 170 (16) @ 4.83 GHz
- **Compiler**: gcc (GCC) 16.2.1 20260810

### 64 bytes cache line
![64](./screenshots/64.png)
It's an actuall cache line length on th machine

### 128 bytes cache line
![128](./screenshots/128.png)

### 256 bytes cache line
![256](./screenshots/256.png)

### 512 bytes cache line
![512](./screenshots/512.png)

It has a boost with insertion or any operation under a bounded count of items (less than ~20'000). It's due to `std::unordered_map` and any other standard map allocates memory per an item. This hash map allocates a cache line for a couple of elements. As we can see, the time of building a map getting as low as cache line size grows. This structure may be useful in a hot cycle, when we have to create a lot of short lifetime maps. 

This structure has a lot of negative sides:
- searching degradation (it's as close to O(N) as it's large)
- only trivially copyable types (the realization uses `memcpy`)

In general standard maps have great api and algorithms. This map can be faster only due to weak api:
- no iterators/pointers searching
- no O(log) operations
- no normal checking (existing before insertion e.g.)

---
But it was fun and useful for my _cmake_ and _gtest_ skills. I have also tried git hooks in the first time.
**I use Arch btw**
