#include <gtest/gtest.h>
#include <memory>
#include <random>
//---------------------------------------------------------------------------
#include "trees/rb_tree.hpp"
//---------------------------------------------------------------------------
using namespace data_structures::rb_tree;
//---------------------------------------------------------------------------
using std::make_unique;
using std::span;
using u32 = uint32_t;
//---------------------------------------------------------------------------
TEST(RBTree, Insert1Get1) {
  auto buffer = make_unique<byte[]>(1024);
  RedBlackTree<u32, u32> rb(span<byte>(buffer.get(), 1024));
  auto *node = rb.insert(1, 2);
  ASSERT_NE(node, nullptr);
  auto found = rb.lookup(1);
  ASSERT_NE(found, nullptr);
  ASSERT_EQ(found->value, 2);
}
//---------------------------------------------------------------------------
TEST(RBTree, ConsecutiveInsert10) {
  const u32 cinsert = 10;
  //---------------------------------------------------------------------------
  auto buffer = make_unique<byte[]>(1024);
  RedBlackTree<u32, u32> rb(span<byte>(buffer.get(), 1024));
  for (u32 i = 0; i < cinsert; ++i) {
    auto node = rb.insert(i, i * 42);
    ASSERT_NE(node, nullptr);
  }
  for (u32 i = 0; i < cinsert; ++i) {
    auto found = rb.lookup(i);
    ASSERT_NE(found, nullptr);
    ASSERT_EQ(found->value, i * 42);
  }
}
//---------------------------------------------------------------------------
TEST(RBTree, RandomInsert10) {
  const u32 cinsert = 10;
  const u32 seed = 12345;
  //---------------------------------------------------------------------------
  auto buffer = make_unique<byte[]>(1024);
  RedBlackTree<u32, u32> rb(span<byte>(buffer.get(), 1024));
  //---------------------------------------------------------------------------
  std::vector<u32> keys(cinsert);
  std::iota(keys.begin(), keys.end(), 0);
  std::mt19937 rng(seed);
  std::shuffle(keys.begin(), keys.end(), rng);
  //---------------------------------------------------------------------------
  for (u32 i = 0; i < cinsert; ++i) {
    u32 key = keys[i];
    auto node = rb.insert(key, key * 42);
    ASSERT_NE(node, nullptr);
  }
  for (u32 i = 0; i < cinsert; ++i) {
    u32 key = keys[i];
    auto found = rb.lookup(key);
    ASSERT_NE(found, nullptr);
    ASSERT_EQ(found->value, key * 42);
  }
}
//---------------------------------------------------------------------------
TEST(RBTree, DuplicateInsert10) {
  const u32 cinsert = 10;
  const u32 seed = 12345;
  //---------------------------------------------------------------------------
  auto buffer = make_unique<byte[]>(1024);
  RedBlackTree<u32, u32> rb(span<byte>(buffer.get(), 1024));
  //---------------------------------------------------------------------------
  std::vector<u32> keys;
  keys.reserve(cinsert);
  std::mt19937 rng(seed);
  std::uniform_int_distribution<u32> dist(0, cinsert - 1);
  for (u32 i = 0; i < cinsert; ++i) {
    keys.push_back(dist(rng));
  }
  //---------------------------------------------------------------------------
  for (u32 i = 0; i < cinsert; ++i) {
    u32 key = keys[i];
    auto node = rb.insert(key, key * 42);
    rb.print();
    ASSERT_NE(node, nullptr);
  }
  for (u32 i = 0; i < cinsert; ++i) {
    u32 key = keys[i];
    auto found = rb.lookup(key);
    ASSERT_NE(found, nullptr);
    ASSERT_EQ(found->value, key * 42);
  }
}