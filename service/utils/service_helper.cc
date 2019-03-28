#include "service_helper.h"
#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <csignal>
#include <string>
#include "unique_id.h"
using chirp::Chirp;
using chirp::Timestamp;
using std::vector;
namespace services {
namespace service_helper {
// convert username, text, id, parent_id to chirp
std::string ChirpInit(const std::string &username, const std::string &text,
                      const std::string &id, const std::string &pid,
                      Timestamp time) {
  std::string chirpstring;
  Chirp chirp{};  // using unique_ptr in here produce bugs!
  chirp.set_username(username);
  chirp.set_text(text);
  chirp.set_id(id);
  chirp.set_parent_id(pid);
  chirp.mutable_timestamp()->set_seconds(time.seconds());
  chirp.mutable_timestamp()->set_useconds(time.useconds());
  chirp.SerializeToString(&chirpstring);
  return chirpstring;
}
// Convert string to chirp pointer
Chirp StringToChirp(const std::string &chirpstring) {
  Chirp chirp;
  chirp.ParseFromString(chirpstring);
  return chirp;
}
}  // namespace service_helper
}  // namespace services
