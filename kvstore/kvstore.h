#ifndef KVSTORE_KVSTORE_H_
#define KVSTORE_KVSTORE_H_
#include <grpcpp/grpcpp.h>
#include "utils/pb/kvstore.grpc.pb.h"
#include "utils/pb/kvstore.pb.h"
#include "utils/cchash.h"
#include <string>
using grpc::ServerContext;
using chirp::DeleteReply;
using chirp::DeleteRequest;
using chirp::GetReply;
using chirp::GetRequest;
using chirp::KeyValueStore;
using chirp::PutReply;
using chirp::PutRequest;
using grpc::Server;
using grpc::Status;
using grpc::ServerReaderWriter;
namespace key_val {
class KeyValueStoreImp1 final : public KeyValueStore::Service {
  // put given element in key value store do not allow to put existed key
  Status put(ServerContext *context, const PutRequest *request,
             PutReply *reply) override;
  // get value through key, if key do not exists return error
  Status get(ServerContext *context,
             ServerReaderWriter<GetReply, GetRequest> *stream) override; 
  // delete a key . if key do not exists, return a error
  Status deletekey(ServerContext *context, const DeleteRequest *request,
                   DeleteReply *reply) override;
 private:
  // database used to store all key-value pairs
  ConcurrentHashTable<std::string, std::string> database_;
};
}  // namespace key_val
#endif //  KVSTORE_KVSTORE_H_
