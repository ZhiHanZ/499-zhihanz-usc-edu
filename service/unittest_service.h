#ifndef UNITTEST_SERVICE_H_
#define UNITTEST_SERVICE_H_
#include <iostream>
#include <algorithm>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include "service_helper.h"
#include "kvstore_client.h"
#include <google/protobuf/message.h>
#include "kvstore.pb.h"
#include "service.pb.h"
#include "service.grpc.pb.h"
#include "kvstore.grpc.pb.h"
#include "../util/UnitTestStatus.h"
#include <grpcpp/grpcpp.h>
#include <queue>

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
using namespace std;
using namespace google::protobuf;
using namespace UnitTest;


class FakeService {
  public:
    FakeCode registeruser(const string request,
                           string reply,
                           UnitTestKVClient& client_){
      if(client_.Has(USER_ID + request) == true){
        return FakeCode{ALREADY_EXISTS};
      }
      auto status1 = client_.Put(USER_ID + request, "");
      if (status1 == false) return FakeCode{NOT_FOUND};
      auto status2 = client_.Put(USER_FOLLOWED + request, "");
      if (status2 == false) return FakeCode{NOT_FOUND};
      return FakeCode{OK};
    }
    auto GetUserId(const string& username, UnitTestKVClient& client_){
      return client_.GetValue(USER_ID + username);
    }
    auto GetUserFollowed(const string& username, UnitTestKVClient& client_) {
      auto followstr = client_.GetValue(USER_FOLLOWED + username);
      return parser::Deparser(followstr);
    }
    auto GetIdReply(const string& id,  UnitTestKVClient& client_){
      auto replystr = client_.GetValue(ID_REPLY + id);
      if (replystr == "") return vector<string>{};
      return parser::Deparser(replystr);
    }
    auto GetIdChirp(const string& id, UnitTestKVClient& client_) {
      return client_.GetValue(ID_CHIRP + id);
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
    FakeCode chirp(const ChirpRequest* request,
                   ChirpReply* reply,
                   UnitTestKVClient& client_){
      auto has_or_not = client_.Has(USER_ID + request->username());
      if(has_or_not == false) return FakeCode{NOT_FOUND};
      auto chirpstring = ChirpStringMaker(request->username(),
                                          request->text(),
                                          request->parent_id());
      Chirp* replyC = helper::StringToChirp(chirpstring);
      reply->set_allocated_chirp(replyC);
      auto id_chirp = client_.Put(ID_CHIRP + replyC->id(), chirpstring);
      if (id_chirp == false) return  FakeCode{ALREADY_EXISTS};
      auto user_id = client_.PutOrUpdate(USER_ID + 
                                         request->username()
                                        , replyC->id());
      if (user_id == false) return FakeCode{ALREADY_EXISTS};
      std::string parent = ID_REPLY + request->parent_id();
      if (client_.Has(parent) == true){
        auto reply = client_.GetValue(parent); 
        std::vector<std::string> replyvec = parser::Deparser(reply);
        replyvec.push_back(replyC->id());
        auto new_reply = parser::Parser(replyvec);
        auto id_reply = client_.PutOrUpdate(parent, new_reply);
        if (id_reply == false) return FakeCode{ALREADY_EXISTS};
        auto id_create = client_.PutOrUpdate(ID_REPLY
                                             + replyC->id(), "");
        if (id_create == false) return FakeCode{ALREADY_EXISTS};
      }else {
        auto id_reply = client_.PutOrUpdate(ID_REPLY
                                             + replyC->id(), "");
        if (id_reply == false) return FakeCode{ALREADY_EXISTS};
      }
      return FakeCode{OK};
    }
    FakeCode follow(const FollowRequest* request, UnitTestKVClient& client_) {
      auto has_or_not = client_.Has(USER_FOLLOWED + request->username());
      if(!has_or_not) return FakeCode{NOT_FOUND};
      has_or_not = client_.Has(USER_FOLLOWED + request->to_follow());
      if(!has_or_not) return FakeCode{NOT_FOUND};
      std::string user = USER_FOLLOWED + request->username();
      std::string followed = USER_FOLLOWED + request->to_follow();
      auto followedRep = client_.GetValue(user);
      std::vector<std::string> followedvec = parser::Deparser(followedRep);
      if (std::find_if(followedvec.begin(), followedvec.end(),
                      [&](const auto& candi)
                      {return candi == request->to_follow();})
                      == followedvec.end()){
        followedvec.push_back(request->to_follow()); 
      }
      auto new_fork = parser::Parser(followedvec);
      auto new_reply = client_.PutOrUpdate(user, new_fork);
      return FakeCode{OK};
    }
    FakeCode read(const ReadRequest* request, ReadReply* reply, UnitTestKVClient& client_) {
      auto readid = request->chirp_id();
      if (!client_.Has(ID_CHIRP + request->chirp_id()))
        return FakeCode{NOT_FOUND};
      std::queue<Chirp> queue{};
      auto root = helper::StringToChirp(GetIdChirp(readid,client_));
      uint64_t index = 0;
      //breadth first search all chirps.
      queue.push(*root);
      while(!queue.empty()){
        Chirp curr = queue.front();
        auto chirpq = reply->add_chirps();
        chirpq[index] = curr;
        cout << curr.username() << endl;
        auto replies = GetIdReply(curr.id(), client_);
        cout << replies.size() << endl;
        if(replies.size() > 0) {
          for (const auto& reply: replies) {
            string chirpstr = GetIdChirp(reply, client_);
            Chirp* chirp = helper::StringToChirp(chirpstr);
            queue.push(*chirp);
          }
        }
        queue.pop();
      }
      return FakeCode{OK};
    }
    FakeCode monitor(const MonitorRequest* request,
                     MonitorReply* reply,
                     UnitTestKVClient& client_){
      using namespace std::this_thread;
      using namespace std::chrono;
      using namespace std::chrono_literals;
      using namespace helper;
      signal(SIGINT, signal_handler);
      auto time_interval = 30ms;
      auto curr = GetMicroSec();
      auto followed = GetUserFollowed(request->username(), client_);
      if (followed.size() == 0){
        return FakeCode{OK};
      }
      while(true) {
        std::this_thread::sleep_for(time_interval);
        for(const auto& f: followed){
          auto curr_id = GetUserId(f,client_);
          auto chirpstr = GetIdChirp(curr_id, client_);
          auto curr_chirp = StringToChirp(chirpstr);
          auto chirp_time = curr_chirp->timestamp();
          if (chirp_time.useconds() > curr) {
            reply->set_allocated_chirp(curr_chirp);
            cout << "Monitor received " <<endl;
            cout << curr_chirp->username() << 
              ": " << curr_chirp->id() << endl;
            cout << curr_chirp->text() << endl;
          }
        }
      }
      return FakeCode{OK};
    }

  private:
    IdGenerator idG;
};
#endif //UNITTEST_SERVICE_H_