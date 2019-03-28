#include "command.h"
using formatChirp::printChirp;
using helper::ChirpRequestMaker;
using helper::FollowRequestMaker;
using helper::MonitorRequestMaker;
using helper::ReadRequestMaker;

static const char DEFAULT_REPLY[]{"-1"};
// Regist a user given a username
Status CommandClient::RegisterUser(const string &registeruser) {
  if (registeruser == "")
    return Status(StatusCode::ALREADY_EXISTS,
                  "there should be at least one chararter for reg");

  ClientContext context;
  RegisterRequest request;
  request.set_username(registeruser);
  RegisterReply reply;
  Status status = stub_->registeruser(&context, request, &reply);
  if (!status.ok()) {
    cout << status.error_message() << endl;
  }
  return status;
}
// Post a chirp
Chirp CommandClient::ChirpPost(const string &username, const string &chirp,
                               const string &reply) {
  ClientContext context;
  auto request = ChirpRequestMaker(username, chirp, reply);
  ChirpReply chirply;
  if (reply != DEFAULT_REPLY) {
    auto request = ReadRequestMaker(reply);
    ClientContext context;
    ReadReply reply;
    auto status = stub_->read(&context, request, &reply);
    if (!status.ok()) {
      cout << "parent id not found" << endl;
      return chirply.chirp();
    }
  }
  auto response = stub_->chirp(&context, request, &chirply);
  if (!response.ok()) {
    cout << response.error_message() << endl;
  }
  return chirply.chirp();
}
// follow to_follow
auto CommandClient::Follow(const string &username, const string &to_follow) {
  auto request = FollowRequestMaker(username, to_follow);
  FollowReply reply;
  ClientContext context;
  auto status = stub_->follow(&context, request, &reply);
  return status;
}
// read a chirp thread
auto CommandClient::Read(const string &id) {
  auto request = ReadRequestMaker(id);
  ClientContext context;
  ReadReply reply;
  auto status = stub_->read(&context, request, &reply);
  if (!status.ok()) {
    cout << status.error_message() << endl;
  }
  return reply;
}
// monitor a people
void CommandClient::Monitor(const string &username) {
  ClientContext context;
  auto request = MonitorRequestMaker(username);
  auto stream = stub_->monitor(&context, *request);
  MonitorReply* reply = new MonitorReply;
  while (stream->Read(reply)) {
    printChirp(reply->chirp());
  }
  auto status = stream->Finish();
  if (!status.ok()) {
    cout << status.error_message() << endl;
  }
}
int main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  LOG(INFO) << "Connected to service layer: localhost:50002";
  CommandClient client(grpc::CreateChannel("localhost:50002",
                                           grpc::InsecureChannelCredentials()));
  LOG(INFO) << "If you want to regist: ./command -regist=<Your name>";
  LOG(INFO) << "If you want to chirp: ./command -user=<Your name> "
               "-chirp=<Your text>";
  LOG(INFO) << "If you want to reply to others: "
            << "./command -user =<Your name>"
            << " -chirp=<Your text>"
            << " -reply=<Reply ID>";
  LOG(INFO) << "If you want to follow somebody: "
            << "./command -user =<Your name>"
            << " -follow=<Other username>";
  LOG(INFO) << "If you want to monitor the followed people for one given user"
            << " ./command -user=<Your name> -monitor";
  // register handler
  if (FLAGS_regist != "" && FLAGS_user == "" && FLAGS_chirp == "" &&
      FLAGS_reply == "" && FLAGS_read == "" && FLAGS_follow == "" &&
      FLAGS_monitor == false) {
    auto status = client.RegisterUser(FLAGS_regist);
    if (!status.ok()) {
      LOG(ERROR) << status.error_message();
    } else {
      LOG(INFO) << "Hello " << FLAGS_regist;
    }
  } else if (FLAGS_regist == "" && FLAGS_user == "" && FLAGS_chirp == "" &&
             FLAGS_reply == "" && FLAGS_read == "" && FLAGS_follow == "" &&
             FLAGS_monitor == false) {
    LOG(INFO) << "PLease Input Valid informations";
  } else if (FLAGS_regist == "" && FLAGS_user != "" && FLAGS_chirp != "" &&
             FLAGS_reply == "" && FLAGS_read == "" && FLAGS_follow == "" &&
             FLAGS_monitor == false) {
    LOG(INFO) << "Sent a chirp post";
    auto chirpreply = client.ChirpPost(FLAGS_user, FLAGS_chirp, "-1");
    printChirp(chirpreply);
  } else if (FLAGS_regist == "" && FLAGS_user != "" && FLAGS_chirp != "" &&
             FLAGS_reply != "" && FLAGS_read == "" && FLAGS_follow == "" &&
             FLAGS_monitor == false) {
    LOG(INFO) << "Reply to " << FLAGS_reply;
    auto chirpreply = client.ChirpPost(FLAGS_user, FLAGS_chirp, FLAGS_reply);
    printChirp(chirpreply);
  } else if (FLAGS_regist == "" && FLAGS_user == "" && FLAGS_chirp == "" &&
             FLAGS_reply == "" && FLAGS_read != "" && FLAGS_follow == "" &&
             FLAGS_monitor == false) {
    LOG(INFO) << "read chirp thread " << FLAGS_read;
    auto readreply = client.Read(FLAGS_read);
    int size = readreply.chirps().size();
    for (auto i = 0; i < size; ++i) {
      printChirp(readreply.chirps(i));
    }
  } else if (FLAGS_regist == "" && FLAGS_user != "" && FLAGS_chirp == "" &&
             FLAGS_follow == "" && FLAGS_reply == "" && FLAGS_read == "" &&
             FLAGS_monitor == true) {
    LOG(INFO) << "monitor on user: " << FLAGS_user;
    client.Monitor(FLAGS_user);
  } else if (FLAGS_regist == "" && FLAGS_user != "" && FLAGS_chirp == "" &&
             FLAGS_follow != "" && FLAGS_reply == "" && FLAGS_read == "" &&
             FLAGS_monitor == false) {
    LOG(INFO) << FLAGS_user << " starts to follow " << FLAGS_follow;
    auto status = client.Follow(FLAGS_user, FLAGS_follow);
    if (!status.ok()) {
      LOG(ERROR) << status.error_message();
    } else {
      LOG(INFO) << "successfully followed " << FLAGS_follow;
    }
  } else {
    LOG(INFO) << "If you want to regist: ./command -regist=<Your name>";
    LOG(INFO) << "If you want to chirp: ./command -user=<Your name> "
                 "-chirp=<Your text>";
    LOG(INFO) << "If you want to reply to others: "
              << "./command -user =<Your name>"
              << " -chirp=<Your text>"
              << " -reply=<Reply ID";
    LOG(INFO) << "If you want to follow somebody: "
              << "./command -user =<Your name>"
              << " -follow=<Other username>";
    LOG(INFO) << "If you want to monitor the followed people for one given user"
              << " ./command -user=<Your name> -monitor";
    LOG(ERROR) << "You should input valid format";
  }
}
