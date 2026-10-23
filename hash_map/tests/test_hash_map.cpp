#include <gtest/gtest.h>

#include <cstdint>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "hash_map/hash_map.hpp"

// Every TEST_F gets a fresh, independent map: tests no longer depend on the
// execution order (safe with --gtest_filter and --gtest_shuffle).
class HashMapTest : public ::testing::Test {
protected:
  using Map = hash_map::HashMap<int, int>;
  Map map;

  // Number of "columns" (keys with equal hash % cache_line_size share one).
  static constexpr int columns = static_cast<int>(hash_map::cache_line_size);

  static int val(int key) { return 2 * key + 1; }

  // Inserts keys 0, stride, 2*stride, ... (n keys). stride == columns puts
  // every key into the SAME column, which forces many chunks.
  void fill(int n, int stride = 1) {
    for (int i = 0; i < n; ++i)
      map.insert(i * stride, val(i * stride));
  }
};

// ============================== CREATE =======================================

TEST_F(HashMapTest, NewMapIsEmpty) {
  EXPECT_TRUE(map.empty());
  EXPECT_EQ(map.size(), 0u);
}

TEST_F(HashMapTest, NewMapContainsNothing) {
  EXPECT_FALSE(map.contains(0));
  EXPECT_FALSE(map.contains(-1));
  EXPECT_FALSE(map.contains(12345));
}

TEST_F(HashMapTest, InsertSingleElement) {
  map.insert(7, 70);
  EXPECT_EQ(map.size(), 1u);
  EXPECT_TRUE(map.contains(7));
  EXPECT_EQ(map.at(7), 70);
  EXPECT_FALSE(map.empty());
}

TEST_F(HashMapTest, InsertFewKeysInDifferentColumns) {
  for (int k = 0; k < 10; ++k)
    map.insert(k, val(k));
  EXPECT_EQ(map.size(), 10u);
  for (int k = 0; k < 10; ++k)
    EXPECT_EQ(map.at(k), val(k));
}

TEST_F(HashMapTest, InsertNegativeKeys) {
  for (int k = -50; k < 0; ++k)
    map.insert(k, val(k));
  EXPECT_EQ(map.size(), 50u);
  for (int k = -50; k < 0; ++k) {
    ASSERT_TRUE(map.contains(k)) << "key " << k;
    EXPECT_EQ(map.at(k), val(k));
  }
}

TEST_F(HashMapTest, DifferentKeyAndValueTypes) {
  hash_map::HashMap<std::uint64_t, double> m;
  for (std::uint64_t k = 0; k < 500; ++k)
    m.insert(k * 1000003ull, static_cast<double>(k) / 4.0);
  EXPECT_EQ(m.size(), 500u);
  for (std::uint64_t k = 0; k < 500; ++k)
    EXPECT_DOUBLE_EQ(m.at(k * 1000003ull), static_cast<double>(k) / 4.0);
}

// ================================ READ =======================================

TEST_F(HashMapTest, ReadThreeChunksWorthOfSequentialKeys) {
  const int count = 3 * columns;
  fill(count);

  EXPECT_EQ(map.size(), static_cast<size_t>(count));
  for (int i = 0; i < count; ++i)
    ASSERT_EQ(map.at(i), val(i)) << "key " << i;
}

TEST_F(HashMapTest, ReadManyKeysAcrossManyChunks) {
  const int count = 10'000;
  fill(count);

  EXPECT_EQ(map.size(), static_cast<size_t>(count));
  for (int i = 0; i < count; ++i)
    ASSERT_EQ(map.at(i), val(i)) << "key " << i;
  for (int i = count; i < count + 100; ++i)
    ASSERT_FALSE(map.contains(i)) << "key " << i;
}

TEST_F(HashMapTest, ReadKeysCollidingIntoOneColumn) {
  // all keys are multiples of `columns` -> same column, many chunks
  const int count = 200;
  fill(count, columns);

  EXPECT_EQ(map.size(), static_cast<size_t>(count));
  for (int i = 0; i < count; ++i)
    ASSERT_EQ(map.at(i * columns), val(i * columns)) << "i " << i;
  // neighbours of those keys (other columns) must not be found
  for (int i = 0; i < count; ++i)
    ASSERT_FALSE(map.contains(i * columns + 1)) << "i " << i;
}

TEST_F(HashMapTest, AtThrowsOutOfRangeOnMissingKey) {
  EXPECT_THROW(map.at(1), std::out_of_range);
  fill(100);
  EXPECT_THROW(map.at(100), std::out_of_range);
  EXPECT_THROW(map.at(-1), std::out_of_range);
}

TEST_F(HashMapTest, ConstAt) {
  fill(100);
  const Map &cmap = map;
  EXPECT_EQ(cmap.at(42), val(42));
  EXPECT_TRUE(cmap.contains(42));
  EXPECT_THROW(cmap.at(1000), std::out_of_range);
}

TEST_F(HashMapTest, ContainsDoesNotModifyMap) {
  fill(50);
  for (int i = 0; i < 1000; ++i)
    (void)map.contains(i);
  EXPECT_EQ(map.size(), 50u);
}

// ============================== UPDATE =======================================

TEST_F(HashMapTest, BracketOverwritesExistingValue) {
  fill(10);
  map[1] = 42;

  EXPECT_EQ(map.at(1), 42);
  EXPECT_EQ(map[1], 42);
  EXPECT_EQ(map.size(), 10u); // no new element appeared
}

TEST_F(HashMapTest, BracketInsertsDefaultForMissingKey) {
  EXPECT_EQ(map[5], 0);
  EXPECT_EQ(map.size(), 1u);
  EXPECT_TRUE(map.contains(5));

  map[5] = 9;
  EXPECT_EQ(map.at(5), 9);
  EXPECT_EQ(map.size(), 1u);
}

TEST_F(HashMapTest, BracketOnEmptyMapThenManyKeys) {
  for (int k = 0; k < 1000; ++k)
    map[k] = val(k);
  EXPECT_EQ(map.size(), 1000u);
  for (int k = 0; k < 1000; ++k)
    ASSERT_EQ(map.at(k), val(k)) << "key " << k;
}

TEST_F(HashMapTest, UpdateEveryValueAcrossChunks) {
  fill(500);
  for (int k = 0; k < 500; ++k)
    map[k] = -k;

  EXPECT_EQ(map.size(), 500u);
  for (int k = 0; k < 500; ++k)
    ASSERT_EQ(map.at(k), -k) << "key " << k;
}

TEST_F(HashMapTest, UpdateDoesNotTouchNeighbours) {
  fill(200);
  map[100] = -1;
  for (int k = 0; k < 200; ++k) {
    int expected = (k == 100) ? -1 : val(k);
    ASSERT_EQ(map.at(k), expected) << "key " << k;
  }
}

TEST_F(HashMapTest, BracketReturnsWritableReference) {
  map[3] = 1;
  int &ref = map[3];
  ref += 10;
  EXPECT_EQ(map.at(3), 11);
}

// ============================== DELETE =======================================

TEST_F(HashMapTest, EraseExistingKey) {
  fill(10);
  map.erase(3);

  EXPECT_FALSE(map.contains(3));
  EXPECT_EQ(map.size(), 9u);
  EXPECT_THROW(map.at(3), std::out_of_range);
}

TEST_F(HashMapTest, EraseMissingKeyIsNoop) {
  fill(10);
  map.erase(999);
  map.erase(-1);
  EXPECT_EQ(map.size(), 10u);
  for (int k = 0; k < 10; ++k)
    EXPECT_EQ(map.at(k), val(k));
}

TEST_F(HashMapTest, EraseFromEmptyMapIsNoop) {
  map.erase(1);
  EXPECT_EQ(map.size(), 0u);
}

TEST_F(HashMapTest, ErasePreservesOtherKeys) {
  const int count = 300;
  fill(count);
  for (int k = 0; k < count; k += 2)
    map.erase(k);

  EXPECT_EQ(map.size(), static_cast<size_t>(count / 2));
  for (int k = 0; k < count; ++k) {
    if (k % 2 == 0) {
      ASSERT_FALSE(map.contains(k)) << "key " << k;
    } else {
      ASSERT_TRUE(map.contains(k)) << "key " << k;
      ASSERT_EQ(map.at(k), val(k)) << "key " << k;
    }
  }
}

TEST_F(HashMapTest, EraseFromCollidingColumn) {
  const int count = 100;
  fill(count, columns);
  for (int i = 0; i < count; i += 3)
    map.erase(i * columns);

  for (int i = 0; i < count; ++i) {
    int key = i * columns;
    if (i % 3 == 0) {
      ASSERT_FALSE(map.contains(key)) << "i " << i;
    } else {
      ASSERT_EQ(map.at(key), val(key)) << "i " << i;
    }
  }
}

TEST_F(HashMapTest, EraseThenReinsertSameKey) {
  map.insert(8, 1);
  map.erase(8);
  map.insert(8, 2);

  EXPECT_EQ(map.size(), 1u);
  EXPECT_EQ(map.at(8), 2);
}

TEST_F(HashMapTest, EraseAllKeys) {
  const int count = 200;
  fill(count);
  for (int k = 0; k < count; ++k)
    map.erase(k);

  EXPECT_EQ(map.size(), 0u);
  for (int k = 0; k < count; ++k)
    ASSERT_FALSE(map.contains(k)) << "key " << k;
}

TEST_F(HashMapTest, EraseTwiceIsSafe) {
  fill(10);
  map.erase(4);
  map.erase(4);
  EXPECT_EQ(map.size(), 9u);
}

TEST_F(HashMapTest, ClearThenReuse) {
  fill(300);
  map.clear();

  EXPECT_EQ(map.size(), 0u);
  EXPECT_FALSE(map.contains(0));

  // the map must stay fully usable after clear()
  fill(50);
  EXPECT_EQ(map.size(), 50u);
  for (int k = 0; k < 50; ++k)
    ASSERT_EQ(map.at(k), val(k)) << "key " << k;
}

// ===================== RANDOMIZED, vs. std::unordered_map ====================

TEST_F(HashMapTest, RandomOperationsMatchUnorderedMap) {
  std::mt19937 rng(12345); // fixed seed -> reproducible failures
  std::uniform_int_distribution<int> key_dist(-2000, 2000);
  std::uniform_int_distribution<int> op_dist(0, 3);

  std::unordered_map<int, int> model;

  for (int step = 0; step < 20'000; ++step) {
    const int key = key_dist(rng);
    switch (op_dist(rng)) {
    case 0: // insert (only new keys: duplicate insert is not supported)
      if (!model.contains(key)) {
        map.insert(key, step);
        model.emplace(key, step);
      }
      break;
    case 1: // erase
      map.erase(key);
      model.erase(key);
      break;
    case 2: // operator[]
      map[key] = step;
      model[key] = step;
      break;
    case 3: // contains
      ASSERT_EQ(map.contains(key), model.contains(key))
          << "step " << step << ", key " << key;
      break;
    }
    ASSERT_EQ(map.size(), model.size()) << "step " << step;
  }

  for (const auto &[k, v] : model) {
    ASSERT_TRUE(map.contains(k)) << "key " << k;
    EXPECT_EQ(map.at(k), v) << "key " << k;
  }
}
