#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <string>

#include "../include/hash_map/hash_map.hpp"

#define CHECKPOINT(msg) std::cout << "==> " << #msg << std::endl;

TEST(CRUD, create) {
  hash_map::HashMap<int, int> hm1;
  CHECKPOINT(created an empty hash map);
}

hash_map::HashMap<int, int> hm1;

TEST(CRUD, read) {
  auto evaluetor = [](int x) { return 2 * x; };
  size_t count = 3 * hash_map::cache_line_size;
  for (size_t i = 0; i < count; ++i) {
    hm1.insert(i, evaluetor(i));
  }

  EXPECT_EQ(count, hm1.size());

  for (size_t i = 0; i < hm1.size(); ++i) {
    EXPECT_EQ(hm1.at(i), evaluetor(i));
  }
  CHECKPOINT(map filled correctly);
}

TEST(CRUD, update) {
  size_t key_to_upd = 1;
  int magic = 42;
  hm1[key_to_upd] = magic;

  EXPECT_EQ(magic, hm1[key_to_upd]);
}

TEST(CRUD, delete) {
  size_t removed = 1 + std::rand() % 32;
  size_t last_size = hm1.size();
  for (size_t i = 0; i < removed; ++i) {
    hm1.erase(2 * i);
  }

  EXPECT_EQ(removed, last_size - hm1.size());
  EXPECT_FALSE(hm1.contains(0));
}
