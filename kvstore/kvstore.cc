#include <memory>
#include <string>
#include <algorithm>
#include<unordered_map>
#include <iostream>
#include <grpcpp/grpcpp.h>
#include "cchash.h"
#include "kvstore.pb.h"
#include "kvstore.grpc.pb.h"
#include "kvhelper.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;
using chirp::GetReply;
using chirp::PutReply;
using chirp::DeleteReply;
using chirp::GetRequest;
using chirp::PutRequest;
using chirp::DeleteRequest;
using chirp::KeyValueStore;
using namespace helper;
//store all variables in database
ConcurrentHashTable<std::string, std::string> database;
//support put(key, value), get(key), delete(key) operation
//operate success return Status::OK
class KeyValueStoreImp1 final : public KeyValueStore::Service {
	Status put(ServerContext* context, const PutRequest* request, PutReply* reply) override {			
		if (Put(database, request->key(), request->value()) == 0){
			return Status::OK;
		}
		else {
			return Status(StatusCode::ALREADY_EXISTS, "key exists.");
		}
	}
	Status get(ServerContext* context, ServerReaderWriter<GetReply,GetRequest>* stream) override {
		GetRequest request;
		while (stream->Read(&request)){
			GetReply response;
			auto replyval = Get(database, request.key());
			if(database.Has(request.key())){
				response.set_value(replyval);
				stream->Write(response);
				return Status::OK;
			}else {
				return Status(StatusCode::ALREADY_EXISTS, "key does not exist.");
			}
		}	
	}
	Status deletekey (ServerContext* context, const DeleteRequest* request, DeleteReply* reply) override {
		if(Delete(database, request->key()) == 0){
			return Status::OK;
		}else {
			return Status(StatusCode::ALREADY_EXISTS, "key does not exist.");
		}
	}
};

void RunServer() {
	std::string server_address{"0.0.0.0:50000"};
	KeyValueStoreImp1 service;
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
}

int main(int argc, char** argv) {
	RunServer();
	return 0;
}
