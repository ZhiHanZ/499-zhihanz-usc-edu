#include <gtest/gtest.h>
#include <string>
#include <unordered_set>
#include <vector>
#include "unique_id.h"
using services::chirp_id::IdGenerator;
using std::string;
using std::unordered_set;
IdGenerator id_generator;  // It will be treated as a singleton in here
// Test whether idGeneration works
TEST(test, idGeneration) {
  auto pair = id_generator();
  auto pair2 = id_generator();
  unordered_set<string> set{pair.second, pair2.second};
  ASSERT_EQ(2, set.size());
}
// Test whether idGeneration works on a huge number of instant generation
TEST(test, idGeneration_big) {
  static const unsigned BIG_NUMBER = 1000;
  unordered_set<string> set;
  for (unsigned i = 0; i < BIG_NUMBER; i++) {
    auto pair = id_generator();
    set.insert(pair.second);
  }
  ASSERT_EQ(BIG_NUMBER, set.size());
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
