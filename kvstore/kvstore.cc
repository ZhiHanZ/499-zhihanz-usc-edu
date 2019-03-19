#include <grpcpp/grpcpp.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "cchash.h"
#include "kvhelper.h"
#include "kvstore.grpc.pb.h"
#include "kvstore.pb.h"

using chirp::DeleteReply;
using chirp::DeleteRequest;
using chirp::GetReply;
using chirp::GetRequest;
using chirp::KeyValueStore;
using chirp::PutReply;
using chirp::PutRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;
using helper::Delete;
using helper::Get;
using helper::Put;
// store all variables in database
ConcurrentHashTable<std::string, std::string> database;
// support put(key, value), get(key), delete(key) operation
// operate success return Status::OK
class KeyValueStoreImp1 final : public KeyValueStore::Service {
  // put given element in key value store do not allow to put existed key
  Status put(ServerContext *context, const PutRequest *request,
             PutReply *reply) override {
    if (Put(database, request->key(), request->value()) == 0) {
      return Status::OK;
    } else {
      return Status(StatusCode::ALREADY_EXISTS, "key exists.");
    }
  }
  // get value through key, if key do not exists return error
  Status get(ServerContext *context,
             ServerReaderWriter<GetReply, GetRequest> *stream) override {
    GetRequest request;
    while (stream->Read(&request)) {
      GetReply response;
      auto replyval = Get(database, request.key());
      if (database.Has(request.key())) {
        response.set_value(replyval);
        stream->Write(response);
        return Status::OK;
      } else {
        return Status(StatusCode::ALREADY_EXISTS, "key does not exist.");
      }
    }
    return Status::OK;
  }
  // delete a key . if key do not exists, return a error
  Status deletekey(ServerContext *context, const DeleteRequest *request,
                   DeleteReply *reply) override {
    if (Delete(database, request->key()) == 0) {
      return Status::OK;
    } else {
      return Status(StatusCode::ALREADY_EXISTS, "key does not exist.");
    }
  }
};

void RunServer() {
  std::string server_address{
      "0.0.0.0:50000"};  // this port is used as key value store
  KeyValueStoreImp1 service;
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char **argv) {
  RunServer();
  return 0;
}
