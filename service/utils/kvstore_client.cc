#include "kvstore_client.h"
#include <vector>
#include "service_helper.h"
#include "pb/kvstore.grpc.pb.h"
#include "pb/kvstore.pb.h"
#include "pb/service.pb.h"
using chirp::Chirp;
using chirp::DeleteReply;
using chirp::DeleteRequest;
using chirp::GetReply;
using chirp::GetRequest;
using chirp::KeyValueStore;
using chirp::PutReply;
using chirp::PutRequest;
using chirp::Timestamp;
namespace services{
// Requests each key in the vector and displays the key and its corresponding
// value as a pair
void KeyValueStoreClient::GetValues(const std::vector<std::string> &keys) {
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
Status KeyValueStoreClient::PutOrUpdate(const std::string &key, const std::string &value) {
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
Status KeyValueStoreClient::Put(const std::string &key, const std::string &value) {
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
std::string KeyValueStoreClient::GetValue(const std::string &key) {
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
Status KeyValueStoreClient::Has(const std::string &key) {
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
Status KeyValueStoreClient::Delete(const std::string &key) {
  DeleteRequest request;
  request.set_key(key);
  DeleteReply reply;
  ClientContext context;
  Status status = stub_->deletekey(&context, request, &reply);
  return status;
}

}  //namespace services

