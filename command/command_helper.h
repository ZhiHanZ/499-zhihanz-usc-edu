#ifndef COMMAND_COMMAND_HELPER_H_
#define COMMAND_COMMAND_HELPER_H_
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include <time.h>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../util/UnitTestStatus.h"
#include "kvstore.grpc.pb.h"
#include "kvstore.pb.h"
#include "service.grpc.pb.h"
#include "service.pb.h"

using chirp::Chirp;
using chirp::ChirpReply;
using chirp::ChirpRequest;
using chirp::DeleteReply;
using chirp::DeleteRequest;
using chirp::FollowReply;
using chirp::FollowRequest;
using chirp::GetReply;
using chirp::GetRequest;
using chirp::KeyValueStore;
using chirp::MonitorReply;
using chirp::MonitorRequest;
using chirp::PutReply;
using chirp::PutRequest;
using chirp::ReadReply;
using chirp::ReadRequest;
using chirp::RegisterReply;
using chirp::RegisterRequest;
using chirp::ServiceLayer;
using chirp::Timestamp;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::StatusCode;
using std::string;
using std::cout;
using std::endl;


DEFINE_string(regist, "", "What username you want to use?");
DEFINE_string(user, "", "please enter ur username");
DEFINE_string(chirp, "", "a new chirp with the given text");
DEFINE_string(reply, "", "please denote a chirp id you want to reply");
DEFINE_string(follow, "", "please enter a username you want to follow");
DEFINE_string(read, "", "read the chirp thread at a given id");
DEFINE_bool(monitor, false, "please enter the user name you want to monitor");
DEFINE_bool(lt, true, "little relation");

namespace helper {
// Make chirp request
static auto ChirpRequestMaker(const string& registeruser, const string& chirp,
                              const string& reply) {
  ChirpRequest* request = new ChirpRequest;
  request->set_username(registeruser);
  request->set_text(chirp);
  request->set_parent_id(reply);
  return request;
}

// Make follow request
static auto FollowRequestMaker(const string& user, const string& tofollow) {
  FollowRequest* request = new FollowRequest;
  request->set_username(user);
  request->set_to_follow(tofollow);
  return request;
}

// Make read request
static auto ReadRequestMaker(const string& id) {
  auto request = new ReadRequest;
  request->set_chirp_id(id);
  return request;
}

// make monitor request
static auto MonitorRequestMaker(const string& user) {
  auto request = new MonitorRequest;
  request->set_username(user);
  return request;
}
}  // namespace helper

namespace formatChirp {
// print formated chirp
static void printChirp(const Chirp& chirp) {
  LOG(INFO) << "username: " << chirp.username();
  LOG(INFO) << "text: " << chirp.text();
  LOG(INFO) << "parent id:  " << chirp.parent_id();
  LOG(INFO) << "Chirp id: " << chirp.id();
  Timestamp time = chirp.timestamp();
  //  thread safety
  time_t sec = time.seconds();
  struct tm newtime;
  localtime_r(&sec, &newtime);
  char buffer[50];
  asctime_r(&newtime, buffer);
  LOG(INFO) << "posted time: " << buffer;
}
}  // namespace formatChirp

#endif  //  COMMAND_COMMAND_HELPER_H_
