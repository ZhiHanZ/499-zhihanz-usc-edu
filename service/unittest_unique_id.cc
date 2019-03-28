#include <gtest/gtest.h>
#include <string>
#include <unordered_set>
#include <vector>
#include "unique_id.h"
using Id::IdGenerator;
using std::cout;
using std::endl;
using std::string;
using std::unordered_set;
using std::vector;
IdGenerator idG;  // It will be treated as a singleton in here
// Test whether idGeneration works
TEST(test, idGeneration) {
  auto pair = idG();
  auto pair2 = idG();
  unordered_set<string> set{pair.second, pair2.second};
  ASSERT_EQ(2, set.size());
}
// Test whether idGeneration works on a huge number of instant generation
TEST(test, idGeneration_big) {
  static const unsigned BIG_NUMBER = 1000;
  unordered_set<string> set;
  for (unsigned i = 0; i < BIG_NUMBER; i++) {
    auto pair = idG();
    set.insert(pair.second);
  }
  ASSERT_EQ(BIG_NUMBER, set.size());
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
