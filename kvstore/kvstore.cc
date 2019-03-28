#include <grpcpp/grpcpp.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "utils/cchash.h"
#include "utils/kvhelper.h"
#include "utils/pb/kvstore.grpc.pb.h"
#include "utils/pb/kvstore.pb.h"
#include "kvstore.h"

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
using key_val::helper::Delete;
using key_val::helper::Get;
using key_val::helper::Put;
namespace key_val {
  // put given element in key value store do not allow to put existed key
  Status KeyValueStoreImp1::put(ServerContext *context, const PutRequest *request,
             PutReply *reply) {
    if (Put(database_, request->key(), request->value()) == 0) {
      return Status::OK;
    } else {
      return Status(StatusCode::ALREADY_EXISTS, "key exists.");
    }
  }
  // get value through key, if key do not exists return error
  Status KeyValueStoreImp1::get(ServerContext *context,
             ServerReaderWriter<GetReply, GetRequest> *stream) {
    GetRequest request;
    while (stream->Read(&request)) {
      GetReply response;
      auto replyval = Get(database_, request.key());
      if (database_.Has(request.key())) {
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
  Status KeyValueStoreImp1::deletekey(ServerContext *context, const DeleteRequest *request,
                   DeleteReply *reply) {
    if (Delete(database_, request->key()) == 0) {
      return Status::OK;
    } else {
      return Status(StatusCode::ALREADY_EXISTS, "key does not exist.");
    }
  }
}  // namespace key_val

void RunServer() {
  std::string server_address{
      "0.0.0.0:50000"};  // this port is used as key value store
  key_val::KeyValueStoreImp1 service;
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
