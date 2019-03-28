#ifndef SERVICE_SERVICE_STRING_H_
#define SERVICE_SERVICE_STRING_H_
#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "utils/kvstore_string.h"
#include "utils/parser.h"
#include "utils/pb/kvstore.pb.h"
#include "utils/pb/service.pb.h"
#include "utils/service_helper.h"
#include "utils/unique_id.h"
#include "utils/unittest_status.h"

using chirp::Chirp;
using chirp::ChirpReply;
using chirp::ChirpRequest;
using chirp::DeleteReply;
using chirp::DeleteRequest;
using chirp::FollowReply;
using chirp::FollowRequest;
using chirp::GetReply;
using chirp::GetRequest;
using chirp::MonitorReply;
using chirp::MonitorRequest;
using chirp::PutReply;
using chirp::PutRequest;
using chirp::ReadReply;
using chirp::ReadRequest;
using chirp::RegisterReply;
using chirp::RegisterRequest;
using chirp::Timestamp;
using services::chirp_id::IdGenerator;
using services::parser::Deparser;
using services::parser::Parser;
using services::service_helper::ChirpInit;
using services::service_helper::ID_CHIRP;
using services::service_helper::ID_REPLY;
using services::service_helper::USER_FOLLOWED;
using services::service_helper::USER_ID;
using std::condition_variable;
using std::mutex;
using std::thread;
using std::vector;
using std::chrono::milliseconds;
using unittest::FakeCode;
using unittest::UnitTestKVClient;
using std::literals::chrono_literals::operator""ms;

namespace unittest {
// This class is the counterpart of original service file in service.h
// Do not use any GRPC communication just used requests and string as Fake
// service For Unit Test Only
class FakeService final {
 public:
  // Get the request user name and send it to client_ database.
  // If register succcessed, return FakeCode{OK}
  // else return FakeCode{{NOT_FOUND}
  FakeCode registeruser(const string &request, string &reply,
                        UnitTestKVClient &client_);
  // Get the most recent published chirp id for given user in fake database
  auto GetUserId(const string &username, const UnitTestKVClient &client_) {
    return client_.GetValue(USER_ID + username);
  }
  // Get the vector of followed user given username
  auto GetUserFollowed(const string &username,
                       const UnitTestKVClient &client_) {
    auto followstr = client_.GetValue(USER_FOLLOWED + username);
    return Deparser(followstr);
  }
  // Get all chirp ids replied to one chirp as a vector
  auto GetIdReply(const string &id, const UnitTestKVClient &client_) {
    auto replystr = client_.GetValue(ID_REPLY + id);
    if (replystr == "") return vector<string>{};
    return Deparser(replystr);
  }
  // Get the chirp corresponding to a chirp id
  auto GetIdChirp(const string &id, const UnitTestKVClient &client_) {
    return client_.GetValue(ID_CHIRP + id);
  }
  // make a chirp string through username, text and parent_id
  string ChirpStringMaker(const string &username, const string &text,
                          const string &parent_id) {
    auto pair = id_generator_();
    auto chirpstring =
        ChirpInit(username, text, pair.second, parent_id, pair.first);
    return chirpstring;
  }
  auto GetNumMonitorLoop() { return monitor_refresh_times_; }
  auto SetNumMonitorLoop(int64_t loop_num) {
    monitor_refresh_times_ = loop_num;
  }
  auto GetRefreshTimeVal() { return refresh_timeval_; }
  auto SetRefreshTimeVal(int wait_time) {
    refresh_timeval_ = milliseconds(wait_time);
  }
  // make a chirp or reply to a chirp through reuqests and store them into
  // databse return the status if send requests successfully return FakeCode{OK}
  FakeCode chirp(const ChirpRequest *request, ChirpReply *reply,
                 UnitTestKVClient &client_);
  // Get the username and to_follow username, let user follow the to_follow
  // people
  FakeCode follow(const FollowRequest *request, UnitTestKVClient &client_);
  // read a chirp thread given chirp id through breadth first search
  FakeCode read(const ReadRequest *request, ReadReply *reply,
                const UnitTestKVClient &client_);
  // Monitor the followed people's chirp given one username
  // 5ms time it will loop over all followed people and send newly published
  // data to chirpReply You can manually set loop number
  FakeCode monitor(const MonitorRequest *request, MonitorReply *reply,
                   const UnitTestKVClient &client);
  // Create another thread put updated monitor reply to vector<Chirp> buffer
  // using mutex monitor_mutex_ and condition_variable monitor_buf_signal_ to
  // synchronize examle usage: If you want to buffer the information from monitor
  // FakeService service;
  // UnitTestKVClient client;
  // MonitorRequest request;
  // MonitorReply reply;
  // std::vector<Chirp> buffer;
  // std::thread buffer_thr = service.MonitorBuffer(&reply, buffer);
  // service.monitor(&request, &reply, client, 100); //timed after about 500 ms
  // buffer_thr.join()
  std::thread MonitorBuffer(const MonitorReply *reply, vector<Chirp> &buffer);
  // allocate chirp messages to reply (in stack(do not need to manage memory;
  void ChirpSet(ChirpReply *reply, const Chirp &chirp);
  // allocate chirp messages to reply (in stack(do not need to manage memory;
  void MonitorSet(MonitorReply *reply, const Chirp &chirp);

 private:
  IdGenerator
      id_generator_;  // dependency injection, This object can generate id
  // the number of loop time in monitor function
  int64_t monitor_refresh_times_ = -1;
  //  poll time interval during
  milliseconds refresh_timeval_ = 5ms;
  // used to synchronize between monitor() and MonitorBuffer function
  // when we need to buffer messages received from monitor
  mutable mutex monitor_mutex_;
  // used to synchronize between monitor() and MonitorBuffer function
  // when we need to buffer messages received from monitor
  condition_variable monitor_buf_signal_;
  // It will be set as True, if monitor received a message
  bool monitor_flag_ = false;
  // It will be set as True, if monitor exit
  bool exit_flag_ = false;
};
}  // namespace unittest

#endif  // SERVICE_SERVICE_STRING_H_
