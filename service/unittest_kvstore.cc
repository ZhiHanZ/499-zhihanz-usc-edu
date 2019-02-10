#include<gtest/gtest.h>
#include <grpcpp/grpcpp.h>
#include<iostream>
#include<string>
#include<vector>
#include "kvstore_client.h"
#include "../kvstore/cchash.h"
#include"kvstore.pb.h"

using chirp::GetReply;
using namespace std;
namespace UnitTest {
using namespace std;
// This is a client implemented for unittest
class UnitTestKVClient {
 public:
  UnitTestKVClient() : table_{} {}
  //Put or update key value pair in client database
  bool PutOrUpdate(std::string key, std::string value) {
    table_.AddOrUpdate(key, value);
    return true;
  }
  //Put the key value pair into key value store
  //if the key exists return false
  bool Put(std::string key, std::string value) {
    if (table_.Has(key)) return false;
    table_.Add(key, value);
    return true;
  }
  //Get the value corresponding to the key
  string GetValue(const std::string& key) {
    string response = table_.GetValue(key);
    return response;
  }
  //Get whether the key value store has such key
  bool Has(std::string key) { return table_.Has(key); }
  //Delete the key value pair given key
  bool Delete(std::string key) {
    table_.DeleteKey(key);
    return true;
  }

  ConcurrentHashTable<string, string> table_;
};

}  // namespace UnitTest
using namespace UnitTest;
ConcurrentHashTable<string, string> table{};
UnitTestKVClient client{};
//Test the put method in client.
TEST(test, Put){
  auto b1 = client.Put("test1", "val1");
  auto b2 = client.Put("test1", "val2");
  EXPECT_EQ(true, b1);
  EXPECT_EQ(false, b2);
  auto val = client.GetValue("test1");
  EXPECT_EQ("val1", val);
}
//Test the PutOrUpdate method in client
TEST(test, Update){
  auto b1 = client.PutOrUpdate("test1", "val2");
  auto b2 = client.PutOrUpdate("test1", "val3");
  EXPECT_EQ(true, b1);
  EXPECT_EQ(true, b2);
  auto val = client.GetValue("test1");
  EXPECT_EQ("val3", val);
}
//Test the delete method in client
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
