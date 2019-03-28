#ifndef SERVICE_SERVICE_H_
#define SERVICE_SERVICE_H_
#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "utils/kvstore_client.h"
#include "utils/pb/kvstore.grpc.pb.h"
#include "utils/pb/kvstore.pb.h"
#include "utils/pb/service.grpc.pb.h"
#include "utils/pb/service.pb.h"
#include "utils/service_helper.h"
#include "utils/unique_id.h"

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
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;
using services::chirp_id::IdGenerator;
using services::service_helper::ID_CHIRP;
using services::service_helper::ID_REPLY;
using services::service_helper::USER_FOLLOWED;
using services::service_helper::USER_ID;
using std::condition_variable;
using std::vector;
using std::chrono::milliseconds;
using std::literals::chrono_literals::operator""ms;

namespace services {
// This class contains all API service can proivde through grpc including
// Register a user to key value store
// allow user to chirp or reply to chirps
// Read a chirp thread through Breath first search
// Follow a chirp user
// Monitor the chirp of followed user
class ServiceImpl final : public ServiceLayer::Service {
 public:
  // Get the number of refresh time during monitor
  // Default value is -1 which means that monitor will run
  // forever util you use ctrl + C to cancel it
  auto GetNumMonitorLoop() { return monitor_refresh_times_; }
  // Set the number of refresh time during monitor
  // you MUST set a number greater than 0;
  auto SetNumMonitorLoop(uint64_t loop_num) {
    monitor_refresh_times_ = loop_num;
  }
  //  Get the refresh time interval, after certain time, scan all follow people
  //  Default refresh time is 5 milliseconds;
  auto GetRefreshTimeVal() { return refresh_timeval_; }
  // Set the refresh time interval in milliseconds
  // Refresh time should always bigger than zero
  auto SetRefreshTimeVal(int wait_time) {
    if (wait_time > 0) {
      refresh_timeval_ = milliseconds(wait_time);
    }
  }
  // register a user by accepting register request and send feedbacks
  Status registeruser(ServerContext *context, const RegisterRequest *request,
                      RegisterReply *reply) override;
  // Get the relevent key value pair to retrive the most recent chirp id pushed
  // buy given user and return the id string
  auto GetUserId(const string &username);
  // Get all users that the given username followed and return a vector of
  // followed people
  auto GetUserFollowed(const string &username);
  // Get all replied chirp is through the given chirp id and return a vector of
  // replied chirp id
  auto GetIdReply(const string &id);
  // Get the corresponding chirp string through chirp id
  auto GetIdChirp(const string &id);
  // Make a chirpstring given username, text, parent_id
  string ChirpStringMaker(const string &username, const string &text,
                          const string &parent_id);
  // Receive username, chirp text, parent_id in request and store the chirp
  // string and relevent information to key value store
  Status chirp(ServerContext *context, const ChirpRequest *request,
               ChirpReply *reply) override;
  // Receive the username and to_follow name in user request and send all
  // information to key value stor
  Status follow(ServerContext *context, const FollowRequest *request,
                FollowReply *reply) override;
  // read the whole chirp thread given chirp id through breadth first search
  Status read(ServerContext *context, const ReadRequest *request,
              ReadReply *reply) override;
  // monitor the chirp sent by followed user.
  // monitor refresh time is 5ms
  // you can optionally pick the number of refresh time by setting loopnumber
  // greter than 0;
  // default number represent a infinite loop and can ONLY return
  // if you press Ctrl + C
  //
  Status monitor(ServerContext *context, const MonitorRequest *request,
                 ServerWriter<MonitorReply> *reply) override;

  // Create another thread put updated monitor reply to vector<Chirp> buffer
  // using mutex monitor_mutex_ and condition_variable monitor_buf_signal_ to
  // synchronize examle usage: If you want to buffer the information from monitor
  // ServiceImpl service;
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
  // dependency injection used to create unique id
  IdGenerator id_generator_;
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
  // It will be set as True, if the information received from buffer should be
  // send to a buffer
  bool buff_mode_ = false;
  // It will be set as True, if monitor received a message
  bool monitor_flag_ = false;
  // It will be set as True, if monitor exit
  bool exit_flag_ = false;
};
}  //  namespace services
#endif  //  SERVICE_SERVICE_H_
