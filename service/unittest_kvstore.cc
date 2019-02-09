#include<gtest/gtest.h>
#include <grpcpp/grpcpp.h>
#include<iostream>
#include<string>
#include<vector>
#include "kvstore_client.h"
#include "../kvstore/cchash.h"
#include"kvstore.pb.h"

using namespace UnitTest;
using chirp::GetReply;
using namespace std;

ConcurrentHashTable<string, string> table{};
UnitTestKVClient client{};
//test that whether Put can put some key value pair into key value store.
TEST(test, Put){
  auto b1 = client.Put("test1", "val1");
  auto b2 = client.Put("test1", "val2");
  EXPECT_EQ(true, b1);
  EXPECT_EQ(false, b2);
  auto val = client.GetValue("test1");
  EXPECT_EQ("val1", val);
}
//PutOrUpdate should add key value pair or update key value pair
TEST(test, Update){
  auto b1 = client.PutOrUpdate("test1", "val2");
  auto b2 = client.PutOrUpdate("test1", "val3");
  EXPECT_EQ(true, b1);
  EXPECT_EQ(true, b2);
  auto val = client.GetValue("test1");
  EXPECT_EQ("val3", val);
}
// Test on whether key value pair can be deleted in key value store
TEST(test, Delete){
  auto b1 = client.Delete("test1");
  EXPECT_EQ(true, b1);
  auto val = client.Has("test1");
  EXPECT_EQ(false, val);
}
int main(int argc, char* argv[]){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
