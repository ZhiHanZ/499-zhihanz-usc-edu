#include "service.h"
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include "utils/parser.h"
#include "utils/service_helper.h"
#include "utils/unique_id.h"
using services::chirp_id::GetMicroSec;
using services::service_helper::ChirpInit;
using services::service_helper::StringToChirp;
using std::literals::chrono_literals::operator""ms;
using services::parser::Deparser;
using services::parser::Parser;
using std::string;
using std::vector;

namespace services {
// Receive a register request and tell the user whether theu are regited
// successfully through status
// const RegisterRequest* request: resgister name,
// return Status::OK if registration succeed;
// return Status(StatusCode::ALREADY_EXISTS,"This username have already used.");
// if regist name already exists;
Status ServiceImpl::registeruser(ServerContext *context,
                                 const RegisterRequest *request,
                                 RegisterReply *reply) {
  KeyValueStoreClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  auto user = request->username();
  if (client.Has(USER_ID + user).ok())
    return Status(StatusCode::ALREADY_EXISTS,
                  "This username have already used.");
  Status status1 = client.Put(USER_ID + user, "");
  if (!status1.ok()) return status1;
  Status status2 = client.Put(USER_FOLLOWED + user, "");
  if (!status2.ok()) return status2;
  return Status::OK;
}
// Get the most recent published chirp id through name
auto ServiceImpl::GetUserId(const string &username) {
  KeyValueStoreClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  return client.GetValue(USER_ID + username);
}
// Get a vector of the people the user followed
auto ServiceImpl::GetUserFollowed(const string &username) {
  KeyValueStoreClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  auto followstr = client.GetValue(USER_FOLLOWED + username);
  return parser::Deparser(followstr);
}
// Get the replied id vector through id
auto ServiceImpl::GetIdReply(const string &id) {
  KeyValueStoreClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  auto replystr = client.GetValue(ID_REPLY + id);
  if (replystr == "") return vector<string>{};
  return parser::Deparser(replystr);
}
// Get the chirp string through id
auto ServiceImpl::GetIdChirp(const string &id) {
  KeyValueStoreClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  return client.GetValue(ID_CHIRP + id);
}
// Make Chirp string
string ServiceImpl::ChirpStringMaker(const string &username, const string &text,
                                     const string &parent_id) {
  auto pair = id_generator_();
  auto chirpstring =
      ChirpInit(username, text, pair.second, parent_id, pair.first);
  return chirpstring;
}
// chirp
// const ChirpRequest *requst: contains username, text and parent id that chirp
// const ChirpReply* reply: contains a complete chirp object
Status ServiceImpl::chirp(ServerContext *context, const ChirpRequest *request,
                          ChirpReply *reply) {
  KeyValueStoreClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  auto has_or_not = client.Has(USER_ID + request->username());
  if (!has_or_not.ok())
    return Status(StatusCode::NOT_FOUND, "user name do not exists");
  auto chirpstring = ChirpStringMaker(request->username(), request->text(),
                                      request->parent_id());
  auto reply_chirp = StringToChirp(chirpstring);
  ChirpSet(reply, reply_chirp);
  auto id_chirp = client.Put(ID_CHIRP + reply_chirp.id(), chirpstring);
  if (!id_chirp.ok())
    return Status(StatusCode::ALREADY_EXISTS, "ID confliction!");
  auto user_id =
      client.PutOrUpdate(USER_ID + request->username(), reply_chirp.id());
  if (!user_id.ok())
    return Status(StatusCode::ALREADY_EXISTS, "USER ID confliction!");
  std::string parent = ID_REPLY + request->parent_id();
  if (client.Has(parent).ok()) {
    auto reply = client.GetValue(parent);
    std::vector<std::string> replyvec = parser::Deparser(reply);
    replyvec.push_back(reply_chirp.id());
    auto new_reply = parser::Parser(replyvec);
    auto id_reply = client.PutOrUpdate(parent, new_reply);
    if (!id_reply.ok())
      return Status(StatusCode::ALREADY_EXISTS, "ID Reply confliction!");
    auto id_create = client.PutOrUpdate(ID_REPLY + reply_chirp.id(), "");
    if (!id_create.ok())
      return Status(StatusCode::ALREADY_EXISTS, "Reply error");
  } else {
    auto id_reply = client.PutOrUpdate(ID_REPLY + reply_chirp.id(), "");
    if (!id_reply.ok())
      return Status(StatusCode::ALREADY_EXISTS, "Reply error");
  }
  return Status::OK;
}
// let user follow to_follow
Status ServiceImpl::follow(ServerContext *context, const FollowRequest *request,
                           FollowReply *reply) {
  KeyValueStoreClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  if (request->username() == request->to_follow())
    return Status(StatusCode::NOT_FOUND, "You cannot Follow yourself");
  auto has_or_not = client.Has(USER_FOLLOWED + request->username());
  if (!has_or_not.ok())
    return Status(StatusCode::NOT_FOUND, "user name do not exists");
  has_or_not = client.Has(USER_FOLLOWED + request->to_follow());
  if (!has_or_not.ok())
    return Status(StatusCode::NOT_FOUND, "followed people do not exists");
  std::string user = USER_FOLLOWED + request->username();
  auto followedRep = client.GetValue(user);
  std::vector<std::string> followedvec = parser::Deparser(followedRep);
  if (std::find_if(followedvec.begin(), followedvec.end(),
                   [&](const auto &candi) {
                     return candi == request->to_follow();
                   }) == followedvec.end()) {
    followedvec.push_back(request->to_follow());
  } else {
    return Status(StatusCode::ALREADY_EXISTS,
                  "You've already followed this guy");
  }
  auto new_fork = parser::Parser(followedvec);
  auto new_reply = client.PutOrUpdate(user, new_fork);
  return Status::OK;
}
// read a chirp thread through BFS
Status ServiceImpl::read(ServerContext *context, const ReadRequest *request,
                         ReadReply *reply) {
  KeyValueStoreClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  auto has_or_not = client.Has(ID_CHIRP + request->chirp_id());
  if (!has_or_not.ok())
    return Status(StatusCode::NOT_FOUND, "chirp id do not exists");
  std::string chirp_id = request->chirp_id();
  std::queue<Chirp> queue{};
  auto root = StringToChirp(GetIdChirp(chirp_id));
  uint64_t index = 0;
  // breadth first search all chirps.
  queue.push(root);
  while (!queue.empty()) {
    Chirp curr = queue.front();
    auto chirpq = reply->add_chirps();
    chirpq[index] = curr;
    auto replies = GetIdReply(curr.id());
    if (replies.size() > 0) {
      for (const auto &reply : replies) {
        string chirpstr = GetIdChirp(reply);
        auto chirp = StringToChirp(chirpstr);
        queue.push(chirp);
      }
    }
    queue.pop();
  }
  return Status::OK;
}
// Watching on currently updated followed people's activities
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
Status ServiceImpl::monitor(ServerContext *context,
                            const MonitorRequest *request,
                            ServerWriter<MonitorReply> *reply) {
  auto time_interval = refresh_timeval_;  // pick one refresh frequency
  auto curr = GetMicroSec();
  auto followed = GetUserFollowed(request->username());
  if (followed.size() == 0) {
    return Status::OK;
  }
  int64_t curr_loop = 0;
  while (curr_loop != monitor_refresh_times_) {
    followed = GetUserFollowed(request->username());
    std::this_thread::sleep_for(time_interval);
    for (const auto &f : followed) {
      auto curr_id = GetUserId(f);
      auto chirpstr = GetIdChirp(curr_id);
      auto curr_chirp = StringToChirp(chirpstr);
      auto chirp_time = curr_chirp.timestamp();
      if (chirp_time.useconds() > curr) {
        std::unique_lock<mutex> monitor_lk(monitor_mutex_);
        if (buff_mode_) {
          monitor_buf_signal_.wait(monitor_lk,
                                   [this] { return !monitor_flag_; });
        }
        MonitorReply monitoreply;
        MonitorSet(&monitoreply, curr_chirp);
        reply->Write(monitoreply);
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
  return Status::OK;
}

// create a new threed to buffer const MonitorReply* reply Chirp data
// vector<Chirp>& buffer is going to buffer Chirp data
std::thread ServiceImpl::MonitorBuffer(const MonitorReply *reply,
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
void ServiceImpl::ChirpSet(ChirpReply *reply, const Chirp &reply_chirp) {
  reply->mutable_chirp()->set_id(reply_chirp.id());
  reply->mutable_chirp()->set_username(reply_chirp.username());
  reply->mutable_chirp()->set_text(reply_chirp.text());
  reply->mutable_chirp()->set_parent_id(reply_chirp.parent_id());
  reply->mutable_chirp()->mutable_timestamp()->set_seconds(
      reply_chirp.timestamp().seconds());
  reply->mutable_chirp()->mutable_timestamp()->set_useconds(
      reply_chirp.timestamp().useconds());
}
void ServiceImpl::MonitorSet(MonitorReply *reply, const Chirp &reply_chirp) {
  reply->mutable_chirp()->set_id(reply_chirp.id());
  reply->mutable_chirp()->set_username(reply_chirp.username());
  reply->mutable_chirp()->set_text(reply_chirp.text());
  reply->mutable_chirp()->set_parent_id(reply_chirp.parent_id());
  reply->mutable_chirp()->mutable_timestamp()->set_seconds(
      reply_chirp.timestamp().seconds());
  reply->mutable_chirp()->mutable_timestamp()->set_useconds(
      reply_chirp.timestamp().useconds());
}
}  // namespace services

void RunServer() {
  std::string server_address{"0.0.0.0:50002"};
  services::ServiceImpl service;
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char **argv) {
  RunServer();
  return 0;
}
