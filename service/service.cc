#include <iostream>
#include <algorithm>
#include <memory>
#include <string>
#include <queue>
#include <vector>
#include "service_helper.h"
#include "kvstore_client.h"
#include <google/protobuf/message.h>
#include "kvstore.pb.h"
#include "service.pb.h"
#include "service.grpc.pb.h"
#include "kvstore.grpc.pb.h"
#include <grpcpp/grpcpp.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
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
using grpc::Status;
using grpc::StatusCode;
using namespace google::protobuf;

const static std::string USER_ID("user_id: ");
const static std::string USER_FOLLOWED("user_followed: ");
const static std::string ID_CHIRP("id_chirp: ");
const static std::string ID_REPLY("id_reply: ");
class ServiceImpl final : public ServiceLayer::Service {
  public:
    Status registeruser (ServerContext* context,
                         const RegisterRequest* request, RegisterReply* reply) override{
      KeyValueStoreClient client(
            grpc::CreateChannel("localhost:50000"
            , grpc::InsecureChannelCredentials()));
      auto user = request->username();
      if(client.Has(USER_ID + user).ok())
        return Status(StatusCode::ALREADY_EXISTS
              , "This username have already used.");
      Status status1 = client.Put(USER_ID + user, "");
      if (!status1.ok()) return status1;
      Status status2 = client.Put(USER_FOLLOWED + user, "");
      if (!status2.ok()) return status2;
      return Status::OK;
    }
    auto GetUserId(const string& username){
      KeyValueStoreClient client(
            grpc::CreateChannel("localhost:50000"
            , grpc::InsecureChannelCredentials()));
      return client.GetValue(USER_ID + username);
    }
    auto GetUserFollowed(const string& username) {
      KeyValueStoreClient client(
            grpc::CreateChannel("localhost:50000"
            , grpc::InsecureChannelCredentials()));
      auto followstr = client.GetValue(USER_FOLLOWED + username);
      return parser::Deparser(followstr);
    }
    auto GetIdReply(const string& id){
      KeyValueStoreClient client(
            grpc::CreateChannel("localhost:50000"
            , grpc::InsecureChannelCredentials()));
      auto replystr = client.GetValue(ID_REPLY + id);
      if (replystr == "") return vector<string>{};
      return parser::Deparser(replystr);
    }
    auto GetIdChirp(const string& id) {
      KeyValueStoreClient client(
            grpc::CreateChannel("localhost:50000"
            , grpc::InsecureChannelCredentials()));
      return client.GetValue(ID_CHIRP + id);
    }
    string ChirpStringMaker(const string& username, 
                            const string& text, 
                            const string& parent_id){
      auto pair = idG();
      auto chirpstring = helper::chirpInit(username,
                                          text, pair.second,
                                          parent_id, pair.first);
      return chirpstring;
    } 
    Status chirp(ServerContext* context, const ChirpRequest* request, ChirpReply* reply)override {
      KeyValueStoreClient client(grpc::CreateChannel(
                                 "localhost:50000", 
                                 grpc::InsecureChannelCredentials()));
      auto has_or_not = client.Has(USER_ID + request->username());
      if(!has_or_not.ok()) return Status(StatusCode::NOT_FOUND, 
                                          "user name do not exists");
      auto chirpstring = ChirpStringMaker(request->username(),
                                          request->text(),
                                          request->parent_id());
      
      Chirp* replyC = helper::StringToChirp(chirpstring);
      reply->set_allocated_chirp(replyC);
      auto id_chirp = client.Put(ID_CHIRP + replyC->id(), chirpstring);
      if (!id_chirp.ok()) return  Status(StatusCode::ALREADY_EXISTS,
                                         "ID confliction!");
      auto user_id = client.PutOrUpdate(USER_ID + 
                                         request->username()
                                        , replyC->id());
      if (!user_id.ok()) return Status(StatusCode::ALREADY_EXISTS,
                                         "USER ID confliction!");
      std::string parent = ID_REPLY + request->parent_id();
      if (client.Has(parent).ok()){
        auto reply = client.GetValue(parent); 
        std::vector<std::string> replyvec = parser::Deparser(reply);
        replyvec.push_back(replyC->id());
        auto new_reply = parser::Parser(replyvec);
        auto id_reply = client.PutOrUpdate(parent, new_reply);
        if (!id_reply.ok()) return Status(StatusCode::ALREADY_EXISTS,
                                         "ID Reply confliction!");
        auto id_create = client.PutOrUpdate(ID_REPLY
                                             + replyC->id(), "");
        if (!id_create.ok()) return Status(StatusCode::ALREADY_EXISTS,
                                           "Reply error");
      }else {
        auto id_reply = client.PutOrUpdate(ID_REPLY
                                             + replyC->id(), "");
        if (!id_reply.ok()) return Status(StatusCode::ALREADY_EXISTS,
                                           "Reply error");
      }
      return Status::OK;
    }
    Status follow(ServerContext* context, const FollowRequest* request, FollowReply* reply) override {
      KeyValueStoreClient client(grpc::CreateChannel(
                                 "localhost:50000", 
                                 grpc::InsecureChannelCredentials()));
      if(request->username() == request->to_follow())
        return Status(StatusCode::NOT_FOUND,
                      "You cannot Follow yourself");
      auto has_or_not = client.Has(USER_FOLLOWED + request->username());
      if(!has_or_not.ok()) return Status(StatusCode::NOT_FOUND, 
                                          "user name do not exists");
      has_or_not = client.Has(USER_FOLLOWED + request->to_follow());
      if(!has_or_not.ok()) return Status(StatusCode::NOT_FOUND, 
                                          "followed people do not exists");
      std::string user = USER_FOLLOWED + request->username();
      auto followedRep = client.GetValue(user);
      std::vector<std::string> followedvec = parser::Deparser(followedRep);
      if (std::find_if(followedvec.begin(), followedvec.end(),
                      [&](const auto& candi)
                      {return candi == request->to_follow();})
                      == followedvec.end()){
        followedvec.push_back(request->to_follow()); 
      }
      auto new_fork = parser::Parser(followedvec);
      auto new_reply = client.PutOrUpdate(user, new_fork);
      return Status::OK;
    }
    Status read(ServerContext* context, 
                const ReadRequest* request, 
                ReadReply* reply)override	{
      KeyValueStoreClient client(grpc::CreateChannel(
                                 "localhost:50000", 
                                 grpc::InsecureChannelCredentials()));
      auto has_or_not = client.Has(ID_CHIRP + request->chirp_id());
      if(!has_or_not.ok()) return Status(StatusCode::NOT_FOUND, 
                                          "chirp id do not exists");
      std::string chirp_id = request->chirp_id();
      std::queue<Chirp> queue{};
      auto root = helper::StringToChirp(GetIdChirp(chirp_id));
      uint64_t index = 0;
      //breadth first search all chirps.
      queue.push(*root);
      while(!queue.empty()){
        Chirp curr = queue.front();
        auto chirpq = reply->add_chirps();
        chirpq[index] = curr;
        cout << curr.username() << endl;
        auto replies = GetIdReply(curr.id());
        cout << replies.size() << endl;
        if(replies.size() > 0) {
          for (const auto& reply: replies) {
            string chirpstr = GetIdChirp(reply);
            Chirp* chirp = helper::StringToChirp(chirpstr);
            queue.push(*chirp);
          }
        }
        queue.pop();
      }
      return Status::OK;
    }
    Status monitor(ServerContext* context, 
                   const MonitorRequest* request, 
                   ServerWriter<MonitorReply>* reply) override {
      using namespace std::this_thread;
      using namespace std::chrono;
      using namespace std::chrono_literals;
      using namespace helper;
      signal(SIGINT, signal_handler);
      auto time_interval = 30ms;//pick one refresh frequency
      auto curr = GetMicroSec();
      auto followed = GetUserFollowed(request->username());
      if(followed.size() == 0) {
        return Status::OK;
      }
      while(true) {
        std::this_thread::sleep_for(time_interval);
        for (const auto& f: followed){
          auto curr_id = GetUserId(f);
          auto chirpstr = GetIdChirp(curr_id);
          auto curr_chirp = StringToChirp(chirpstr);
          auto chirp_time = curr_chirp->timestamp();
          if (chirp_time.useconds() > curr) {
            MonitorReply* monitoreply = new MonitorReply;
            monitoreply->set_allocated_chirp(curr_chirp);
            reply->Write(*monitoreply);
            curr = chirp_time.useconds();
          }
        }
      }
      return Status::OK;
    }
  private:
    helper::IdGenerator idG;
};

void RunServer() {
  std::string server_address{"0.0.0.0:50002"};
  ServiceImpl service;
  ServerBuilder builder; 
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();
  return 0;
}
