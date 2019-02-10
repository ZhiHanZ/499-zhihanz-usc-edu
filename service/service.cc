#include "service.h"

//Receive a register request and tell the user whether theu are regited successfully through status
Status ServiceImpl::registeruser (ServerContext* context,
                     const RegisterRequest* request, RegisterReply* reply){
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
//Get the most recent published chirp id through name
auto ServiceImpl::GetUserId(const string& username){
  KeyValueStoreClient client(
        grpc::CreateChannel("localhost:50000"
        , grpc::InsecureChannelCredentials()));
  return client.GetValue(USER_ID + username);
}
//Get a vector of the people the user followed
auto ServiceImpl::GetUserFollowed(const string& username) {
  KeyValueStoreClient client(
        grpc::CreateChannel("localhost:50000"
        , grpc::InsecureChannelCredentials()));
  auto followstr = client.GetValue(USER_FOLLOWED + username);
  return parser::Deparser(followstr);
}
//Get the replied id vector through id
auto ServiceImpl::GetIdReply(const string& id){
  KeyValueStoreClient client(
        grpc::CreateChannel("localhost:50000"
        , grpc::InsecureChannelCredentials()));
  auto replystr = client.GetValue(ID_REPLY + id);
  if (replystr == "") return vector<string>{};
  return parser::Deparser(replystr);
}
//Get the chirp string through id 
auto ServiceImpl::GetIdChirp(const string& id) {
  KeyValueStoreClient client(
        grpc::CreateChannel("localhost:50000"
        , grpc::InsecureChannelCredentials()));
  return client.GetValue(ID_CHIRP + id);
}
//Make Chirp string
string ServiceImpl::ChirpStringMaker(const string& username, 
                        const string& text, 
                        const string& parent_id){
  auto pair = idG();
  auto chirpstring = helper::chirpInit(username,
                                      text, pair.second,
                                      parent_id, pair.first);
  return chirpstring;
} 
//chirp 
Status ServiceImpl::chirp(ServerContext* context, const ChirpRequest* request, ChirpReply* reply){
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
//let user follow to_follow
Status ServiceImpl::follow(ServerContext* context, const FollowRequest* request, FollowReply* reply) {
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
//read a chirp thread through BFS
Status ServiceImpl::read(ServerContext* context, 
            const ReadRequest* request, 
            ReadReply* reply) {
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
//Watching on currently updated followed people's activities
Status ServiceImpl::monitor(ServerContext* context, 
               const MonitorRequest* request, 
               ServerWriter<MonitorReply>* reply) {
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
        MonitorReply monitoreply;
        monitoreply.set_allocated_chirp(curr_chirp);
        reply->Write(monitoreply);
        curr = chirp_time.useconds();
      }
    }
  }
  return Status::OK;
}

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
