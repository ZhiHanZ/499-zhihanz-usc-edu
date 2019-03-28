#include "command.h"
#include "utils/command_helper.h"
using command_helper::CommandHandler;
using command_helper::kCHIRP;
using command_helper::kFOLLOW;
using command_helper::kMONITOR;
using command_helper::kNULL;
using command_helper::kOTHERS;
using command_helper::kREAD;
using command_helper::kREGIST;
using command_helper::kREPLY;
using format::FollowHandler;
using format::HelpInfo;
using format::PrintChirp;
using format::RegistHandler;
using helper::ChirpRequestMaker;
using helper::FollowRequestMaker;
using helper::MonitorRequestMaker;
using helper::ReadRequestMaker;
DEFINE_string(regist, "", "What username you want to use?");
DEFINE_string(user, "", "please enter ur username");
DEFINE_string(chirp, "", "a new chirp with the given text");
DEFINE_string(reply, "", "please denote a chirp id you want to reply");
DEFINE_string(follow, "", "please enter a username you want to follow");
DEFINE_string(read, "", "read the chirp thread at a given id");
DEFINE_bool(monitor, false, "please enter the user name you want to monitor");
DEFINE_bool(lt, true, "little relation");
namespace commandline {
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
  auto stream = stub_->monitor(&context, request);
  MonitorReply reply;
  while (stream->Read(&reply)) {
    PrintChirp(reply.chirp());
  }
  auto status = stream->Finish();
  if (!status.ok()) {
    cout << status.error_message() << endl;
  }
}

}  // namespace commandline
int main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  LOG(INFO) << "Connected to service layer: localhost:50002";
  commandline::CommandClient client(grpc::CreateChannel(
      "localhost:50002", grpc::InsecureChannelCredentials()));
  HelpInfo();
  uint32_t mode =
      CommandHandler(FLAGS_regist, FLAGS_user, FLAGS_chirp, FLAGS_reply,
                     FLAGS_read, FLAGS_follow, FLAGS_monitor);
  if (mode == kREGIST) {
    auto status = client.RegisterUser(FLAGS_regist);
    RegistHandler(status, FLAGS_regist);
  } else if (mode == kNULL) {
    LOG(INFO) << "PLease Input Valid informations";
  } else if (mode == kCHIRP) {
    LOG(INFO) << "Sent a chirp post";
    auto chirpreply = client.ChirpPost(FLAGS_user, FLAGS_chirp, "-1");
    PrintChirp(chirpreply);
  } else if (mode == kREPLY) {
    LOG(INFO) << "Reply to " << FLAGS_reply;
    auto chirpreply = client.ChirpPost(FLAGS_user, FLAGS_chirp, FLAGS_reply);
    PrintChirp(chirpreply);
  } else if (mode == kREAD) {
    LOG(INFO) << "read chirp thread " << FLAGS_read;
    auto readreply = client.Read(FLAGS_read);
    int size = readreply.chirps().size();
    for (auto i = 0; i < size; ++i) {
      PrintChirp(readreply.chirps(i));
    }
  } else if (mode == kMONITOR) {
    LOG(INFO) << "monitor on user: " << FLAGS_user;
    client.Monitor(FLAGS_user);
  } else if (mode == kFOLLOW) {
    LOG(INFO) << FLAGS_user << " starts to follow " << FLAGS_follow;
    auto status = client.Follow(FLAGS_user, FLAGS_follow);
    FollowHandler(status, FLAGS_follow);
  } else {
    HelpInfo();
  }
}
