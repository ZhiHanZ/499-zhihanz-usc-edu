#ifndef SERVICE_H_
#define SERVICE_H_
#include "kvstore.grpc.pb.h"
#include "kvstore.pb.h"
#include "kvstore_client.h"
#include "service.grpc.pb.h"
#include "service.pb.h"
#include "service_helper.h"
#include <algorithm>
#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <vector>

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
using namespace google::protobuf;
using namespace std::this_thread;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace helper;

const static std::string USER_ID("user_id: ");
const static std::string USER_FOLLOWED("user_followed: ");
const static std::string ID_CHIRP("id_chirp: ");
const static std::string ID_REPLY("id_reply: ");

// This class contains all API service can proivde through grpc including
// Register a user to key value store
// allow user to chirp or reply to chirps
// Read a chirp thread through Breath first search
// Follow a chirp user
// Monitor the chirp of followed user
class ServiceImpl final : public ServiceLayer::Service {
public:
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
  Status monitor(ServerContext *context, const MonitorRequest *request,
                 ServerWriter<MonitorReply> *reply) override;

private:
  IdGenerator idG_;
};
#endif
