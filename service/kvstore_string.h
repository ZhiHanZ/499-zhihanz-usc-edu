
#ifndef SERVICE_KVSTORE_STRING_H_
#define SERVICE_KVSTORE_STRING_H_
#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include "../kvstore/cchash.h"
#include "../util/UnitTestStatus.h"
#include "kvstore.pb.h"
#include "service.pb.h"
#include "service_helper.h"
#include "unique_id.h"

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
using Id::GetMicroSec;
using Id::IdGenerator;
using std::cout;
using std::endl;
using std::literals::chrono_literals::operator""ms;

namespace unittest {
// The kvstore client which is used for unittest
// Support Put, PutOrUpdate, GetValue through key, Delete key value pair through
// key operations
class UnitTestKVClient final {
 public:
  UnitTestKVClient() : table_{} {}
  // Put or update the key value pair in key value store
  bool PutOrUpdate(const std::string &key, const std::string &value) {
    table_.AddOrUpdate(key, value);
    return true;
  }
  // Put the key value pair into key value store
  // If store has given key return false
  bool Put(const std::string &key, const std::string &value) {
    if (table_.Has(key)) return false;
    table_.Add(key, value);
    return true;
  }
  // Get the value of corresponding key
  string GetValue(const std::string &key) const {
    string response = table_.GetValue(key);
    return response;
  }
  // return whether key value store has such key
  bool Has(const std::string &key) const { return table_.Has(key); }
  // delete corresponding key value pair through key
  bool Delete(std::string key) {
    table_.DeleteKey(key);
    return true;
  }

 private:
  ConcurrentHashTable<string, string>
      table_;  // Hash Table injected for the purpose of test
};
}  // namespace unittest

#endif  // SERVICE_KVSTORE_STRING_H_
