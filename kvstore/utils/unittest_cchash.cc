#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "cchash.h"
using utils::ConcurrentHashTable;
ConcurrentHashTable<std::string, std::string> hs;
// test whether test can be inited successfully
TEST(test, init) { ASSERT_EQ(hs.GetValue("test2", "undefined"), "undefined"); }
// test whether we can add elements (if key exists, do nothing)
TEST(test, add) {
  hs.AddOrUpdate("test1", "test1");
  ASSERT_EQ(hs.GetValue("test1", "undefined"), "test1");
  ASSERT_EQ(hs.GetValue("test2", "undefined"), "undefined");
  hs.AddOrUpdate("test2", "test2");
  ASSERT_EQ(hs.GetValue("test2", "undefined"), "test2");
  hs.Add("add1", "add");
  ASSERT_EQ(hs.GetValue("add1"), "add");
}
// test whether we can update value through key
TEST(test, update) {
  hs.AddOrUpdate("test3", " 3 ");
  hs.AddOrUpdate("test3", " 4 ");
  hs.AddOrUpdate("test3", " 5 ");
  hs.AddOrUpdate("test3", " 6 ");
  ASSERT_EQ(hs.GetValue("test3", "undefined"), " 6 ");
}
// test whether we can delete them successfully
TEST(test, deletes) {
  ASSERT_EQ(hs.GetValue("test2", "undefined"), "test2");
  hs.DeleteKey("test2");
  ASSERT_EQ(hs.GetValue("test2", "undefined"), "undefined");
}
// test has method which will return true if key value store contain such value
TEST(test, has) {
  ASSERT_EQ(hs.Has("test2"), false);
  ASSERT_EQ(hs.Has("test3"), true);
  ASSERT_EQ(hs.Has("test4"), false);
}
int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
