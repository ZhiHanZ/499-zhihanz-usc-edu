#ifndef COMMAND_HELPER_H_
#define COMMAND_HELPER_H_
#include <iostream>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <ctime>
#include <vector>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "kvstore.grpc.pb.h"
#include "kvstore.pb.h"
#include "service.pb.h"
#include "service.grpc.pb.h"
#include "../util/UnitTestStatus.h"

using namespace std;
using grpc::Channel;
using grpc::ClientContext;
using chirp::RegisterRequest;
using chirp::RegisterReply;
using chirp::ChirpRequest;
using chirp::ChirpReply;
using chirp::Chirp;
using chirp::Timestamp;
using chirp::FollowRequest;
using chirp::FollowReply;
using chirp::ReadRequest;
using chirp::ReadReply;
using chirp::MonitorRequest;
using chirp::MonitorReply;
using chirp::ServiceLayer;
using chirp::GetReply;
using chirp::PutReply;
using chirp::DeleteReply;
using chirp::GetRequest;
using chirp::PutRequest;
using chirp::DeleteRequest;
using chirp::KeyValueStore;
using chirp::ServiceLayer;
using grpc::Status;
using grpc::StatusCode;
using namespace google::protobuf;

DEFINE_string(regist,"", "What username you want to use?");
DEFINE_string(user, "", "please enter ur username");
DEFINE_string(chirp, "", "a new chirp with the given text");
DEFINE_string(reply, "", "please denote a chirp id you want to reply");
DEFINE_string(follow, "", "please enter a username you want to follow");
DEFINE_string(read, "", "read the chirp thread at a given id");
DEFINE_bool(monitor, false, "please enter the user name you want to monitor");
DEFINE_bool(lt, true, "little relation");
const static string VOIDREGISTERERROR("username should contain at least one chararacter");
const static string SUCCESS("success");
const static string FAIL("some thing is wrong");
namespace helper {
  //Make chirp request
  static auto ChirpRequestMaker(const string& registeruser,
                                const string& chirp, 
                                const string& reply){
    ChirpRequest* request = new ChirpRequest;
    request->set_username(registeruser);
    request->set_text(chirp);
    request->set_parent_id(reply);
    return request;
  }
  //Make follow request
  static auto FollowRequestMaker(const string& user,
                                 const string& tofollow){
    FollowRequest* request = new FollowRequest;
    request->set_username(user);
    request->set_to_follow(tofollow);
    return request;
  }
  //Make read request
  static auto ReadRequestMaker(const string& id){
    auto request = new ReadRequest;
    request->set_chirp_id(id);
    return request;
  }
  //make monitor request
  static auto MonitorRequestMaker(const string& user){
    auto request = new MonitorRequest;
    request->set_username(user);
    return request;
  }
}
namespace formatChirp {
  //print formated chirp
  static void printChirp(const Chirp& chirp) {
    LOG(INFO) << "username: " << chirp.username();
    LOG(INFO) << "text: " << chirp.text();
    LOG(INFO) << "parent id:  " << chirp.parent_id();
    LOG(INFO) << "Chirp id: " << chirp.id();
    Timestamp time = chirp.timestamp();
    time_t sec = time.seconds();
    LOG(INFO) << "posted time: " << asctime(localtime(&sec));
  }
}

#endif
