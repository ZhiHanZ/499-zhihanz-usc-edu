#ifndef COMMAND_UTILS_COMMAND_HELPER_H_
#define COMMAND_UTILS_COMMAND_HELPER_H_
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include <time.h>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "pb/kvstore.grpc.pb.h"
#include "pb/kvstore.pb.h"
#include "pb/service.grpc.pb.h"
#include "pb/service.pb.h"

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
using std::cout;
using std::endl;
using std::string;

namespace command_helper {
static const uint32_t kREGIST = 1;
static const uint32_t kCHIRP = 2;
static const uint32_t kREPLY = 3;
static const uint32_t kFOLLOW = 4;
static const uint32_t kMONITOR = 5;
static const uint32_t kREAD = 6;
static const uint32_t kNULL = 7;
static const uint32_t kOTHERS = 8;
static const uint32_t kSTREAM = 9;

// switch to different case when encounter different inputs`
uint32_t CommandHandler(const string& regist, const string& user,
                        const string& chirp, const string& reply,
                        const string& read, const string& follow,
                        const bool& monitor, const string& stream);
}  // namespace command_helper
namespace helper {
// Make chirp request
static auto ChirpRequestMaker(const string& registeruser, const string& chirp,
                              const string& reply) {
  ChirpRequest request;
  request.set_username(registeruser);
  request.set_text(chirp);
  request.set_parent_id(reply);
  return request;
}

// Make follow request
static auto FollowRequestMaker(const string& user, const string& tofollow) {
  FollowRequest request;
  request.set_username(user);
  request.set_to_follow(tofollow);
  return request;
}

// Make read request
static auto ReadRequestMaker(const string& id) {
  ReadRequest request;
  request.set_chirp_id(id);
  return request;
}

// make monitor request
static auto MonitorRequestMaker(const string& user) {
  MonitorRequest request;
  request.set_username(user);
  return request;
}
}  //  namespace helper

namespace format {
// print formated chirp
void PrintChirp(const Chirp& chirp);
// print help information
void HelpInfo();
// print regist information
void RegistHandler(const Status& status, const string& regist);
// print follow information
void FollowHandler(const Status& status, const string& follow);
}  // namespace format

#endif  //  COMMAND_UTILS_COMMAND_HELPER_H_
