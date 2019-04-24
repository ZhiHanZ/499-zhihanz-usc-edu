#include <gtest/gtest.h>
#include <chrono>
#include <functional>
#include <string>
#include <vector>
#include "service_string.h"
#include "utils/kvstore_string.h"
#include "utils/service_helper.h"
#include "utils/unittest_status.h"

using services::service_helper::StringToChirp;
using std::string;
using std::vector;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using unittest::FakeCode;
using unittest::FakeService;
using unittest::UnitTestKVClient;
using unittest::FakeCode::ALREADY_EXISTS;
using unittest::FakeCode::NOT_FOUND;
using unittest::FakeCode::OK;
using std::literals::chrono_literals::operator""ms;
namespace unittest {
// Can send a chirp automaticly given name time in ms and the number of chirps
void AutoChirp(FakeService& service, string name, int micro, int chirp_num,
               UnitTestKVClient& client) {
  int i = 0;
  while (i != chirp_num) {
    string text{"Hello World + " + std::to_string(i)};
    string parent_id{"-1"};
    ChirpRequest request;
    ChirpReply reply;
    request.set_username(name);
    request.set_text(text);
    request.set_parent_id(parent_id);
    std::this_thread::sleep_for(milliseconds(micro));
    service.chirp(&request, &reply, client);
    i++;
  }
}
}  // namespace unittest
using unittest::AutoChirp;
UnitTestKVClient client;
FakeService service;
//  test whether we can regist a user in kvstore
TEST(test, regist) {
  string request{"Adam"};
  string reply;
  auto status = service.registeruser(request, reply, client);
  ASSERT_EQ(FakeCode{OK}, status);
  status = service.registeruser(request, reply, client);
  ASSERT_EQ(FakeCode{ALREADY_EXISTS}, status);
}
//  test whether basic chirp works
TEST(test, chirp) {
  string name{"Adam"};
  string text{"Hello World"};
  string parent_id{"-1"};
  ChirpRequest request;
  ChirpReply reply;
  request.set_username(name);
  request.set_text(text);
  request.set_parent_id(parent_id);
  auto status = service.chirp(&request, &reply, client);
  ASSERT_EQ(FakeCode{OK}, status);
  ASSERT_EQ(request.username(), reply.chirp().username());
  ASSERT_EQ(request.text(), reply.chirp().text());
  ASSERT_EQ(request.parent_id(), reply.chirp().parent_id());
  // Test whether GetUserId works
  ASSERT_EQ(service.GetUserId(name, client), reply.chirp().id());
  // Test whether GetIdChirp works
  auto chirp_reply = reply.chirp();
  auto chirpstr_found = service.GetIdChirp(reply.chirp().id(), client);
  auto chirp_found = StringToChirp(chirpstr_found);
  ASSERT_EQ(chirp_reply.id(), chirp_found.id());
  ASSERT_EQ(chirp_reply.username(), chirp_found.username());
  ASSERT_EQ(chirp_reply.text(), chirp_found.text());
  ASSERT_EQ(chirp_reply.parent_id(), chirp_found.parent_id());
}
//  test a logic : if user do not registed, get FakeCode{NOT_FOUND}
TEST(test, chirp_not_regist) {
  string name{"Unknow"};
  string text{"Hello World"};
  string parent_id{"-1"};
  ChirpRequest request;
  ChirpReply reply;
  request.set_username(name);
  request.set_text(text);
  request.set_parent_id(parent_id);
  auto status = service.chirp(&request, &reply, client);
  ASSERT_EQ(FakeCode{NOT_FOUND}, status);
}
//  test whether we can reply to a chirp
TEST(test, chirp_reply) {
  // regist a new user
  string regist_request{"Smith"};
  string regist_reply;
  auto status = service.registeruser(regist_request, regist_reply, client);
  ASSERT_EQ(FakeCode{OK}, status);
  // Get the chirp Id I want
  string poster{"Adam"};
  auto chirpid_found = service.GetUserId(poster, client);
  // Chirp
  string name{regist_request};
  string text{"Hello Adam"};
  string parent_id{chirpid_found};
  ChirpRequest request;
  ChirpReply reply;
  request.set_username(name);
  request.set_text(text);
  request.set_parent_id(parent_id);
  status = service.chirp(&request, &reply, client);
  ASSERT_EQ(FakeCode{OK}, status);
  ASSERT_EQ(request.username(), reply.chirp().username());
  ASSERT_EQ(request.text(), reply.chirp().text());
  ASSERT_EQ(request.parent_id(), reply.chirp().parent_id());
  ASSERT_EQ(chirpid_found, reply.chirp().parent_id());
  // test whether GetIdReply works
  auto replyvec = service.GetIdReply(chirpid_found, client);
  ASSERT_EQ(replyvec[0], reply.chirp().id());
}
//  test whether multiple reply to a chirp works

TEST(test, chirp_multi_reply) {
  // Get the chirp Id I want
  string poster{"Adam"};
  auto chirpid_found = service.GetUserId(poster, client);
  // reply 100 chirps to given id
  static const int CHIRP_NUM = 100;
  for (int i = 0; i < CHIRP_NUM; i++) {
    // Chirp
    string name{"Smith"};
    string text{std::to_string(i)};
    string parent_id{chirpid_found};
    ChirpRequest request;
    ChirpReply reply;
    request.set_username(name);
    request.set_text(text);
    request.set_parent_id(parent_id);
    auto status = service.chirp(&request, &reply, client);
    ASSERT_EQ(FakeCode{OK}, status);
    ASSERT_EQ(request.username(), reply.chirp().username());
    ASSERT_EQ(request.text(), reply.chirp().text());
    ASSERT_EQ(request.parent_id(), reply.chirp().parent_id());
    ASSERT_EQ(chirpid_found, reply.chirp().parent_id());
  }
  auto replyvec = service.GetIdReply(chirpid_found, client);
  ASSERT_EQ(101, replyvec.size());
}

// test whether follow functiion works
TEST(test, follow) {
  FollowRequest request;
  request.set_username("Smith");
  request.set_to_follow("Adam");
  auto status = service.follow(&request, client);
  ASSERT_EQ(FakeCode{OK}, status);
  auto followed_vec = service.GetUserFollowed(request.username(), client);
  ASSERT_EQ(1, followed_vec.size());
  ASSERT_EQ("Adam", followed_vec[0]);
  request.set_username("Smith");
  request.set_to_follow("Adam");
  status = service.follow(&request, client);
  ASSERT_EQ(FakeCode{OK}, status);
  followed_vec = service.GetUserFollowed(request.username(), client);
  ASSERT_EQ(1, followed_vec.size());
  ASSERT_EQ("Adam", followed_vec[0]);
}
// if user not found, follow will return FakeCode{NOT_FOUND}
TEST(test, follow_not_found) {
  FollowRequest request;
  request.set_username("Unknown");
  request.set_to_follow("Adam");
  auto status = service.follow(&request, client);
  ASSERT_EQ(FakeCode{NOT_FOUND}, status);
  request.set_username("Adam");
  request.set_to_follow("Unknown");
  status = service.follow(&request, client);
  ASSERT_EQ(FakeCode{NOT_FOUND}, status);
  request.set_username("Adam");
  request.set_to_follow("Adam");
  status = service.follow(&request, client);
  ASSERT_EQ(FakeCode{NOT_FOUND}, status);
}

// test basic function of read
TEST(test, read) {
  string poster{"Adam"};
  auto chirpid_found = service.GetUserId(poster, client);
  ReadRequest request;
  ReadReply reply;
  request.set_chirp_id(chirpid_found);
  auto status = service.read(&request, &reply, client);
  ASSERT_EQ(FakeCode{OK}, status);
  EXPECT_EQ(102, reply.chirps_size());
  EXPECT_EQ("Adam", reply.chirps(0).username());
  EXPECT_EQ(chirpid_found, reply.chirps(0).id());
  EXPECT_EQ("-1", reply.chirps(0).parent_id());
  EXPECT_EQ("Hello World", reply.chirps(0).text());
  EXPECT_EQ("Hello Adam", reply.chirps(1).text());
  EXPECT_EQ("Smith", reply.chirps(1).username());
  EXPECT_EQ(chirpid_found, reply.chirps(1).parent_id());
  for (int i = 0; i < 100; i++) {
    EXPECT_EQ(std::to_string(i), reply.chirps(i + 2).text());
    EXPECT_EQ("Smith", reply.chirps(i + 2).username());
    EXPECT_EQ(chirpid_found, reply.chirps(i + 2).parent_id());
  }
}
// test basic function of monitor
TEST(test, monitor) {
  vector<Chirp> buffer;
  MonitorRequest monitor_request;
  MonitorReply monitor_reply;
  monitor_request.set_username("Smith");
  std::thread th1(AutoChirp, std::ref(service), "Adam", 100, 3,
                  std::ref(client));
  //  A modified version of monitor which will automatically exit
  //  if time passed 510 ms in here
  auto th2 = service.MonitorBuffer(&monitor_reply, buffer);
  service.SetNumMonitorLoop(110);
  service.OpenBuffer();
  service.monitor(&monitor_request, &monitor_reply, client);
  // Set the value in promise
  // Wait for thread to join
  th1.join();
  th2.join();
  service.CloseBuffer();
  EXPECT_EQ(3, buffer.size());
  // it will put monitor into default value;
  EXPECT_EQ(-1, service.GetNumMonitorLoop());
  EXPECT_EQ(5ms, service.GetRefreshTimeVal());
  int i = 0;
  for (auto v : buffer) {
    EXPECT_EQ("Adam", v.username());
    auto textstr = "Hello World + " + std::to_string(i);
    EXPECT_EQ(textstr, v.text());
    i++;
  }
}

// test basic function of stream
TEST(test, stream) {
  vector<Chirp> buffer;
  MonitorRequest monitor_request;
  MonitorReply monitor_reply;
  monitor_request.set_username("Smith");
  std::thread th1(AutoChirp, std::ref(service), "Adam", 100, 3,
                  std::ref(client));
  //  A modified version of monitor which will automatically exit
  //  if time passed 510 ms in here
  auto th2 = service.MonitorBuffer(&monitor_reply, buffer);
  service.SetNumMonitorLoop(110);
  service.OpenBuffer();
  service.monitor(&monitor_request, &monitor_reply, client);
  // Set the value in promise
  // Wait for thread to join
  th1.join();
  th2.join();
  service.CloseBuffer();
  EXPECT_EQ(3, buffer.size());
  // it will put monitor into default value;
  EXPECT_EQ(-1, service.GetNumMonitorLoop());
  EXPECT_EQ(5ms, service.GetRefreshTimeVal());
  int i = 0;
  for (auto v : buffer) {
    EXPECT_EQ("Adam", v.username());
    auto textstr = "Hello World + " + std::to_string(i);
    EXPECT_EQ(textstr, v.text());
    i++;
  }
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
