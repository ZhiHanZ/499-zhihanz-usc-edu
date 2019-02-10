#ifndef KVSTORE_CLIENT_H_
#define KVSTORE_CLIENT_H_

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

class KeyValueStoreClient {
 public:
  KeyValueStoreClient(std::shared_ptr<Channel> channel)
      : stub_(KeyValueStore::NewStub(channel)) {}
  // Put or Update database
  Status PutOrUpdate(std::string key, std::string value) {
    PutRequest request;
    request.set_key(key);
    request.set_value(value);
    PutReply reply;
    ClientContext context;
    Status status = stub_->put(&context, request, &reply);
    return status;
  }
  // Put key value into database
  // if key exists, return status
  Status Put(std::string key, std::string value) {
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
  // Get a value through key
  std::string GetValue(const std::string& key) {
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
  // Return whether kvstore has a key
  Status Has(std::string key) {
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
  // Delete a key
  Status Delete(std::string key) {
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


#endif
