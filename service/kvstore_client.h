#ifndef KVSTORE_CLIENT_H_
#define KVSTORE_CLIENT_H_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "kvstore.grpc.pb.h"
#include "kvstore.pb.h"
#include "service.pb.h"
#include "../service/service_helper.h"
#include "../kvstore/cchash.h"

using grpc::Channel;
using grpc::ClientContext;
using chirp::GetReply;
using chirp::PutReply;
using chirp::DeleteReply;
using chirp::GetRequest;
using chirp::PutRequest;
using chirp::Chirp;
using chirp::Timestamp;
using chirp::DeleteRequest;
using chirp::KeyValueStore;
using grpc::StatusCode;
using grpc::Status;

class KeyValueStoreClient {
 public:
  KeyValueStoreClient(std::shared_ptr<Channel> channel)
      : stub_(KeyValueStore::NewStub(channel)) {}

  // Requests each key in the vector and displays the key and its corresponding
  // value as a pair
  void GetValues(const std::vector<std::string>& keys) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    auto stream = stub_->get(&context);
    for (const auto& key : keys) {
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
	Status PutOrUpdate(std::string key, std::string value) {
		PutRequest request;
		request.set_key(key);
		request.set_value(value);
		PutReply reply;
		ClientContext context;
		Status status = stub_->put(&context, request, &reply);
    return status;
	}
	Status Put(std::string key, std::string value) {
    if (Has(key).ok()) {return Status(StatusCode::ALREADY_EXISTS, "key exists");}
		PutRequest request;
		request.set_key(key);
		request.set_value(value);
		PutReply reply;
		ClientContext context;
		Status status = stub_->put(&context, request, &reply);
    return status;
	}
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

namespace UnitTest {
  using namespace std;
  class UnitTestKVClient {
    public:
      UnitTestKVClient() :table_{}{}
      bool PutOrUpdate(std::string key, std::string value) {
        table_.AddOrUpdate(key, value);
        return true;
      }
      bool Put(std::string key, std::string value) {
        if(table_.Has(key)) return false;
        table_.Add(key, value);
        return true;
      }
      string GetValue(const std::string& key) {
        string response = table_.GetValue(key);
        return response;
      }
      bool Has(std::string key) {
        return table_.Has(key);
      }
      bool Delete(std::string key) {
        table_.DeleteKey(key);
        return true;
      }

      ConcurrentHashTable<string, string> table_;
  };

}

#endif
