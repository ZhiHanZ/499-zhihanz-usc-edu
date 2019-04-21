#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "parser.h"
#include "service_helper.h"
#include "unique_id.h"
using services::chirp_id::IdGenerator;
using services::parser::Deparser;
using services::parser::Parser;
using services::service_helper::ChirpInit;
using services::service_helper::StringToChirp;
using std::cout;
using std::endl;
using std::string;
using std::unordered_set;
using std::vector;

IdGenerator idG;
// parse username, text, pair , pid into a chirp string and convert it back to
// see whether it can be converted successfully
TEST(test, chirpInit) {
  auto username = "Adam";
  auto text = "Hello World";
  auto pair = idG();
  auto pid = "-1";
  auto hashtag = "-1";
  auto str = ChirpInit(username, text, pair.second, pid, pair.first, hashtag);
  Chirp chirp;
  chirp.ParseFromString(str);
  ASSERT_EQ(username, chirp.username());
  ASSERT_EQ(text, chirp.text());
  ASSERT_EQ(pid, chirp.parent_id());
  ASSERT_EQ(pair.second, chirp.id());
}
// Test whether chirp-string conversion works
TEST(test, chirpstring) {
  auto username = "Smith";
  auto text = "Hello World";
  auto pair = idG();
  auto pid = "-1";
  auto hashtag = "-1";
  auto str = ChirpInit(username, text, pair.second, pid, pair.first, hashtag);
  Chirp chirp;
  chirp.ParseFromString(str);
  Chirp chirp2 = StringToChirp(str);
  ASSERT_EQ(chirp2.username(), chirp.username());
  ASSERT_EQ(chirp2.text(), chirp.text());
  ASSERT_EQ(chirp2.parent_id(), chirp.parent_id());
  ASSERT_EQ(chirp2.id(), chirp.id());
}
// This test is not relevent to the development and should be deleted
int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
