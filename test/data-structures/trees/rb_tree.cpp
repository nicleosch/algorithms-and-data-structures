#include <gtest/gtest.h>
#include <memory>
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
TEST(RBTree, Insert4Consecutive) {
  const u32 cinsert = 4;
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