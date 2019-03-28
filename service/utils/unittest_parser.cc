#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "parser.h"
using services::parser::Deparser;
using services::parser::Parser;
using std::cout;
using std::endl;
using std::string;
using std::vector;

// test whether parser and deparser can be used to the empty string and empty
// vector
TEST(test, init) {
  std::string str{};
  std::vector<string> vec{};
  auto str1 = Parser(vec);
  ASSERT_EQ("", str1);
  auto vec1 = Deparser(str1);
  ASSERT_EQ(vec, vec1);
}
// test a simple case of vector to string and convert back to see whether they
// are equal
TEST(test, add) {
  std::string add1("n12hd1oiuh!!");
  std::vector<string> vec{"test1^*(())", "test2h1ue91**~~~!!!",
                          "HJVYUFOIUAMOIQ*!&((*&)Y*)"};
  auto str1 = Parser(vec);
  auto vec1 = Deparser(str1);
  ASSERT_EQ(vec, vec1);
}
// This test is not relevent to the development and should be deleted
int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
