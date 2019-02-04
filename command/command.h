#ifndef COMMAND_H_
#define COMMAND_H_
#include"command_helper.h"

using namespace helper;
class CommandClient {
  public:
    CommandClient(std::shared_ptr<Channel> channel)
      : stub_(ServiceLayer::NewStub(channel)) {}
    //Register a user
    Status RegisterUser(const string& registeruser){
      if(registeruser == "")
        return Status(StatusCode::ALREADY_EXISTS, "there should be at least one chararter for reg");

      ClientContext context;
      RegisterRequest* request = new RegisterRequest;
      request->set_username(registeruser);
      RegisterReply reply;
      Status status = stub_->registeruser(&context, *request, &reply);
      if (!status.ok()) {
        cout << status.error_message() << endl;
      }
      return status;
    }
    //Post a chirp
    Chirp ChirpPost(const string& username, const string& chirp, const string& reply){
      ClientContext context;
      auto request = ChirpRequestMaker(username, chirp, reply);
      auto chirply = new ChirpReply;
      auto response =  stub_->chirp(&context, *request, chirply);
      if(!response.ok()) {
        cout << response.error_message() << endl;
      }
      return chirply->chirp();
    }
    //follow to_follow 
    auto Follow(const string& username, const string& to_follow) {
      auto request = FollowRequestMaker(username, to_follow);
      FollowReply reply;
      ClientContext context;
      auto status = stub_->follow(&context, *request, &reply);
      return status;
    }
    //read a chirp thread
    auto Read(const string& id){
      auto request = ReadRequestMaker(id);
      ClientContext context;
      ReadReply* reply = new ReadReply;
      auto status = stub_->read(&context, *request, reply);
      if(!status.ok()){
        cout << status.error_message() << endl;
      }
      return *reply;
    }
    //monitor a people
    void Monitor(const string& username) {
      ClientContext context;
      auto request = MonitorRequestMaker(username);
      auto stream  = stub_->monitor(&context, *request);
      MonitorReply* reply = new MonitorReply;
      while(stream->Read(reply)){
        //will stream block thread and wait for response after received a response
        //sent it to reply?
        printChirp(reply->chirp());
      }
      auto status = stream->Finish();
      if (!status.ok()){
        cout <<status.error_message() << endl;
      }
    }
  private:
    std::unique_ptr<ServiceLayer::Stub> stub_;

};
#endif //COMMAND_H_
