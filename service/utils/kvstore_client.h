#ifndef SERVICE_KVSTORE_CLIENT_H_
#define SERVICE_KVSTORE_CLIENT_H_

#include <grpcpp/grpcpp.h>
#include <string>
#include "pb/kvstore.grpc.pb.h"

using chirp::KeyValueStore;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::StatusCode;

namespace services {
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
  void GetValues(const std::vector<std::string> &keys); 
  // Put or Update given key value pair to database
  Status PutOrUpdate(const std::string &key, const std::string &value); 
  // Put the given key value pair to database
  // If database has given key return StautusCode:;ALREADY_EXISTS status
  Status Put(const std::string &key, const std::string &value);
  // Get the value through key in key value store
  std::string GetValue(const std::string &key); 
  // Return the status shows that whether database has such key
  Status Has(const std::string &key); 
  // Delete a key value pair in databse through key
  Status Delete(const std::string &key); 

 private:
  std::unique_ptr<KeyValueStore::Stub> stub_;
};

}
#endif  // SERVICE_KVSTORE_CLIENT_H_
