#include "command.h"
int main(int argc, char* argv[]){
	google::ParseCommandLineFlags(&argc, &argv, true);
	LOG(INFO) << "Connected to service layer: localhost:50002";
  CommandClient client(grpc::CreateChannel("localhost:50002", grpc::InsecureChannelCredentials()));
  //register handler
  if (FLAGS_regist != "" && FLAGS_user == "" && FLAGS_chirp == "" 
      && FLAGS_reply == "" && FLAGS_read =="" && FLAGS_follow == "" && FLAGS_monitor==false) {
    auto status = client.RegisterUser(FLAGS_regist);
    if (!status.ok()) {
      LOG(ERROR) << status.error_message();
    }else {
      LOG(INFO) << "Hello " << FLAGS_regist;
    }
  }else if (FLAGS_regist == "" && FLAGS_user == "" && FLAGS_chirp == "" 
            && FLAGS_reply=="" && FLAGS_read =="" && FLAGS_follow == "" && FLAGS_monitor==false) {
    LOG(INFO) << "PLease Input Valid informations";
  }else if (FLAGS_regist == "" && FLAGS_user != "" && FLAGS_chirp != "" 
            && FLAGS_reply=="" && FLAGS_read =="" && FLAGS_follow == "" && FLAGS_monitor==false) {
    LOG(INFO) << "Sent a chirp post";
    auto chirpreply = client.ChirpPost(FLAGS_user, FLAGS_chirp, "-1");
    printChirp(chirpreply);
  }else if (FLAGS_regist == "" && FLAGS_user != "" && FLAGS_chirp != "" 
            && FLAGS_reply !="" && FLAGS_read =="" && FLAGS_follow == "" && FLAGS_monitor==false){
    LOG(INFO) << "Reply to " << FLAGS_reply;
    auto chirpreply = client.ChirpPost(FLAGS_user, FLAGS_chirp, FLAGS_reply);
    printChirp(chirpreply);
  }else if (FLAGS_regist == "" && FLAGS_user == "" && FLAGS_chirp == "" 
            && FLAGS_reply =="" && FLAGS_read !="" && FLAGS_follow == "" && FLAGS_monitor==false){
    LOG(INFO) << "read chirp thread " << FLAGS_read;
    auto readreply = client.Read(FLAGS_read);
    int size = readreply.chirps().size();
    for(auto i = 0; i < size; ++i) {
      printChirp(readreply.chirps(i));
    }
  }else if (FLAGS_regist == "" && FLAGS_user != "" && FLAGS_chirp == "" && FLAGS_follow == ""  
            && FLAGS_reply =="" && FLAGS_read =="" && FLAGS_monitor == true){
    LOG(INFO) << "monitor on user: " << FLAGS_user;
    client.Monitor(FLAGS_user);
  } else if (FLAGS_regist == "" && FLAGS_user != "" && FLAGS_chirp == "" && FLAGS_follow != ""  
            && FLAGS_reply =="" && FLAGS_read =="" && FLAGS_monitor == false) {
    LOG(INFO) <<FLAGS_user << " starts to follow " << FLAGS_follow;
    auto status = client.Follow(FLAGS_user, FLAGS_follow);
    if (!status.ok()){
      LOG(ERROR) << status.error_message();
    }
    else {
      LOG(INFO) << "successfully followed " << FLAGS_follow;
    }
  }
  else {
    LOG(ERROR) << "You should input valid format";
  }
}
