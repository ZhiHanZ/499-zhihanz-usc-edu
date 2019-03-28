#include "service_string.h"
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include "utils/kvstore_string.h"
#include "utils/parser.h"
#include "utils/pb/kvstore.pb.h"
#include "utils/pb/service.pb.h"
#include "utils/service_helper.h"
#include "utils/unique_id.h"

using services::chirp_id::GetMicroSec;
using services::chirp_id::IdGenerator;
using services::service_helper::StringToChirp;
using std::literals::chrono_literals::operator""ms;
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
using grpc::Channel;
using grpc::ClientContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;
using services::parser::Deparser;
using services::parser::Parser;
using std::condition_variable;
using std::mutex;
using std::chrono::milliseconds;
using unittest::FakeCode;
using unittest::UnitTestKVClient;
namespace unittest {
// const string& request: resgister name,
// string& client: client that will be used to regist
// return FakeCode{OK} if registration succeed.
// return FakeCode{ALReADY_EXISTS} if registration failed
FakeCode FakeService::registeruser(const string &request, string &reply,
                                   UnitTestKVClient &client) {
  if (client.Has(USER_ID + request) == true) {
    return FakeCode{ALREADY_EXISTS};
  }
  auto status1 = client.Put(USER_ID + request, "");
  if (status1 == false) return FakeCode{NOT_FOUND};
  auto status2 = client.Put(USER_FOLLOWED + request, "");
  if (status2 == false) return FakeCode{NOT_FOUND};
  return FakeCode{OK};
}
// const ChirpRequest *requst: contains username, text and parent id that chirp
FakeCode FakeService::chirp(const ChirpRequest *request, ChirpReply *reply,
                            UnitTestKVClient &client) {
  auto has_or_not = client.Has(USER_ID + request->username());
  if (has_or_not == false) return FakeCode{NOT_FOUND};
  auto chirpstring = ChirpStringMaker(request->username(), request->text(),
                                      request->parent_id());
  auto reply_chirp = StringToChirp(chirpstring);
  ChirpSet(reply, reply_chirp);
  auto id_chirp = client.Put(ID_CHIRP + reply_chirp.id(), chirpstring);
  if (id_chirp == false) {
    return FakeCode{ALREADY_EXISTS};
  }
  auto user_id =
      client.PutOrUpdate(USER_ID + request->username(), reply_chirp.id());
  if (user_id == false) {
    return FakeCode{ALREADY_EXISTS};
  }
  std::string parent = ID_REPLY + request->parent_id();
  // if parent id exists, add this chirp's id to its parent id's child list
  if (client.Has(parent) == true) {
    auto reply = client.GetValue(parent);
    std::vector<std::string> replyvec = Deparser(reply);
    replyvec.push_back(reply_chirp.id());
    auto new_reply = Parser(replyvec);
    auto id_reply = client.PutOrUpdate(parent, new_reply);
    if (id_reply == false) {
      return FakeCode{ALREADY_EXISTS};
    }
    auto id_create = client.PutOrUpdate(ID_REPLY + reply_chirp.id(), "");
    if (id_create == false) {
      return FakeCode{ALREADY_EXISTS};
    }
  } else {
    // no parents
    auto id_reply = client.PutOrUpdate(ID_REPLY + reply_chirp.id(), "");
    if (id_reply == false) {
      return FakeCode{ALREADY_EXISTS};
    }
  }
  return FakeCode{OK};
}
// const FollowRequest *request contains user and to-follow people
FakeCode FakeService::follow(const FollowRequest *request,
                             UnitTestKVClient &client) {
  if (request->username() == request->to_follow()) return FakeCode{NOT_FOUND};
  auto has_or_not = client.Has(USER_FOLLOWED + request->username());
  if (!has_or_not) return FakeCode{NOT_FOUND};
  has_or_not = client.Has(USER_FOLLOWED + request->to_follow());
  if (!has_or_not) return FakeCode{NOT_FOUND};
  std::string user = USER_FOLLOWED + request->username();
  std::string followed = USER_FOLLOWED + request->to_follow();
  auto followedRep = client.GetValue(user);
  std::vector<std::string> followedvec = Deparser(followedRep);
  // Find whether the followed people in the follow list
  if (std::find_if(followedvec.begin(), followedvec.end(),
                   [&](const auto &candi) {
                     return candi == request->to_follow();
                   }) == followedvec.end()) {
    followedvec.push_back(request->to_follow());
  }
  auto new_fork = Parser(followedvec);
  client.PutOrUpdate(user, new_fork);
  return FakeCode{OK};
}
// read a chirp thread from client database
// const ReadRequest* request contains the chirp id to read
// ReadReply* reply contains all chirps in that thread
// It will allocate Chirp object during function call
// and it is read function's responsibility to delete it.
FakeCode FakeService::read(const ReadRequest *request, ReadReply *reply,
                           const UnitTestKVClient &client) {
  auto readid = request->chirp_id();
  if (!client.Has(ID_CHIRP + request->chirp_id())) return FakeCode{NOT_FOUND};
  std::queue<Chirp> queue{};
  // Allocated Chirp* object
  auto root = StringToChirp(GetIdChirp(readid, client));
  uint64_t index = 0;
  // breadth first search all chirps.
  queue.push(root);
  // Delete root chirp
  while (!queue.empty()) {
    Chirp curr = queue.front();
    auto chirpq = reply->add_chirps();
    chirpq[index] = curr;
    auto replies = GetIdReply(curr.id(), client);
    if (replies.size() > 0) {
      for (const auto &reply : replies) {
        string chirpstr = GetIdChirp(reply, client);
        Chirp chirp = StringToChirp(chirpstr);  // allocated Chirp*
        queue.push(chirp);
      }
    }
    queue.pop();
  }
  return FakeCode{OK};
}
// monitor all follow people's chirp
// const MonitorRequst *request contains the username we want to monitor
// once received updated chirp, it will send to MonitorReply* reply
// loop times represent the number of loops monitor want to use
// default value of loop times is -1 represents loop forever
// Class private variable std::mutex monitor_mutex_
// std::condition_variable monitor_buf_signal_
// bool monitor_flag_ and end_flag_ are used to synchronize monitor
// and its buffer function MonitorBuffer.
// it is monitor's responsibility for curr_chirp object
FakeCode FakeService::monitor(const MonitorRequest *request,
                              MonitorReply *reply,
                              const UnitTestKVClient &client) {
  auto time_interval = refresh_timeval_;
  auto curr = GetMicroSec();
  auto followed = GetUserFollowed(request->username(), client);
  if (followed.size() == 0) {
    return FakeCode{OK};
  }
  int64_t curr_loop = 0;
  while (curr_loop != monitor_refresh_times_) {
    followed = GetUserFollowed(request->username(), client);
    std::this_thread::sleep_for(time_interval);
    for (const auto &f : followed) {
      auto curr_id = GetUserId(f, client);
      auto chirpstr = GetIdChirp(curr_id, client);
      auto curr_chirp = StringToChirp(chirpstr);  // allocated
      auto chirp_time = curr_chirp.timestamp();
      // once received updated data notify MonitorBuffer
      if (chirp_time.useconds() > curr) {
        std::unique_lock<mutex> monitor_lk(monitor_mutex_);
        monitor_buf_signal_.wait(monitor_lk, [this] { return !monitor_flag_; });
        MonitorSet(reply, curr_chirp);
        monitor_flag_ = true;
        monitor_lk.unlock();
        monitor_buf_signal_.notify_one();
        curr = chirp_time.useconds();
      }
    }
    // avoid of overflow
    if (monitor_refresh_times_ != -1) {
      curr_loop++;
    }
  }
  // once it existed, using exit_flag_ to notify MonitorBuffer to exit.
  std::lock_guard<std::mutex> lk(monitor_mutex_);
  exit_flag_ = true;
  monitor_refresh_times_ = -1;
  monitor_buf_signal_.notify_one();
  return FakeCode{OK};
}
// create a new threed to buffer const MonitorReply* reply Chirp data
// vector<Chirp>& buffer is going to buffer Chirp data
std::thread FakeService::MonitorBuffer(const MonitorReply *reply,
                                       vector<Chirp> &buffer) {
  // it will wake up this thread
  auto lock_cond = [this] { return exit_flag_ || monitor_flag_; };
  std::thread thr([this, reply, lock_cond, &buffer] {
    while (true) {
      std::unique_lock<mutex> monitor_lk(monitor_mutex_);
      monitor_buf_signal_.wait(monitor_lk, lock_cond);
      if (exit_flag_) return;
      Chirp curr = reply->chirp();
      buffer.push_back(curr);
      monitor_flag_ = false;
      monitor_lk.unlock();
      monitor_buf_signal_.notify_all();
    }
  });
  return thr;
}
void FakeService::ChirpSet(ChirpReply *reply, const Chirp &reply_chirp) {
  reply->mutable_chirp()->set_id(reply_chirp.id());
  reply->mutable_chirp()->set_username(reply_chirp.username());
  reply->mutable_chirp()->set_text(reply_chirp.text());
  reply->mutable_chirp()->set_parent_id(reply_chirp.parent_id());
  reply->mutable_chirp()->mutable_timestamp()->set_seconds(
      reply_chirp.timestamp().seconds());
  reply->mutable_chirp()->mutable_timestamp()->set_useconds(
      reply_chirp.timestamp().useconds());
}
void FakeService::MonitorSet(MonitorReply *reply, const Chirp &reply_chirp) {
  reply->mutable_chirp()->set_id(reply_chirp.id());
  reply->mutable_chirp()->set_username(reply_chirp.username());
  reply->mutable_chirp()->set_text(reply_chirp.text());
  reply->mutable_chirp()->set_parent_id(reply_chirp.parent_id());
  reply->mutable_chirp()->mutable_timestamp()->set_seconds(
      reply_chirp.timestamp().seconds());
  reply->mutable_chirp()->mutable_timestamp()->set_useconds(
      reply_chirp.timestamp().useconds());
}

}  // namespace unittest
