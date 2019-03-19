#ifndef SERVICE_KVSTORE_CLIENT_H_
#define SERVICE_KVSTORE_CLIENT_H_

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../kvstore/cchash.h"
#include "../service/service_helper.h"
#include "kvstore.grpc.pb.h"
#include "kvstore.pb.h"
#include "service.pb.h"

using chirp::Chirp;
using chirp::DeleteReply;
using chirp::DeleteRequest;
using chirp::GetReply;
using chirp::GetRequest;
using chirp::KeyValueStore;
using chirp::PutReply;
using chirp::PutRequest;
using chirp::Timestamp;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::StatusCode;

// KeyValue Store Client is used by service layer to communicate with key value
// store database Support Put or Update key value pair Put given key value pair
// API Get Value through key operation tell server whether key value store has
// given key and delete key value pair through given in key value store
// operations
class KeyValueStoreClient {
 public:
  explicit KeyValueStoreClient(std::shared_ptr<Channel> channel)
                              : stub_(KeyValueStore::NewStub(channel)) {}

  // Requests each key in the vector and displays the key and its corresponding
  // value as a pair
  void GetValues(const std::vector<std::string> &keys) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    auto stream = stub_->get(&context);
    for (const auto &key : keys) {
      // Key we are sending to the server.
      GetRequest request;
      request.set_key(key);
      stream->Write(request);
      // Get the value for the sent key
      GetReply response;
      stream->Read(&response);
    }
    stream->WritesDone();
    Status status = stream->Finish();
    if (!status.ok()) {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
    }
  }
  // Put or Update given key value pair to database
  Status PutOrUpdate(const std::string &key, const std::string &value) {
    PutRequest request;
    request.set_key(key);
    request.set_value(value);
    PutReply reply;
    ClientContext context;
    Status status = stub_->put(&context, request, &reply);
    return status;
  }
  // Put the given key value pair to database
  // If database has given key return StautusCode:;ALREADY_EXISTS status
  Status Put(const std::string &key, const std::string &value) {
    if (Has(key).ok()) {
      return Status(StatusCode::ALREADY_EXISTS, "key exists");
    }
    PutRequest request;
    request.set_key(key);
    request.set_value(value);
    PutReply reply;
    ClientContext context;
    Status status = stub_->put(&context, request, &reply);
    return status;
  }
  // Get the value through key in key value store
  std::string GetValue(const std::string &key) {
    ClientContext context;
    auto stream = stub_->get(&context);
    GetRequest request;
    request.set_key(key);
    stream->Write(request);
    // Get the value for the sent key
    GetReply response;
    stream->Read(&response);
    stream->WritesDone();
    Status status = stream->Finish();
    return response.value();
  }
  // Return the status shows that whether database has such key
  Status Has(const std::string &key) {
    ClientContext context;
    auto stream = stub_->get(&context);
    GetRequest request;
    request.set_key(key);
    stream->Write(request);
    // Get the value for the sent key
    GetReply response;
    stream->Read(&response);
    stream->WritesDone();
    Status status = stream->Finish();
    return status;
  }
  // Delete a key value pair in databse through key
  Status Delete(const std::string &key) {
    DeleteRequest request;
    request.set_key(key);
    DeleteReply reply;
    ClientContext context;
    Status status = stub_->deletekey(&context, request, &reply);
    return status;
  }

 private:
  std::unique_ptr<KeyValueStore::Stub> stub_;
};
#endif  // SERVICE_KVSTORE_CLIENT_H_
