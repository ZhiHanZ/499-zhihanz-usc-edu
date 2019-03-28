#ifndef SERVICE_UTILS_SERVICE_HELPER_H_
#define SERVICE_UTILS_SERVICE_HELPER_H_

#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <string>
#include <vector>
#include "pb/service.pb.h"
using chirp::Chirp;

namespace services {
namespace service_helper {
//prefix during service key value store communication
static const char USER_ID[]{"user_id: "};
static const char USER_FOLLOWED[]{"user_followed: "};
static const char ID_CHIRP[]{"id_chirp: "};
static const char ID_REPLY[]{"id_reply: "};

// convert username, text, id, parent_id to chirp
std::string ChirpInit(const std::string &username, const std::string &text,
                      const std::string &id, const std::string &pid,
                      chirp::Timestamp time);
// Convert string to chirp pointer
Chirp StringToChirp(const std::string &chirpstring);
}
}  // namespace service_helper
#endif  //  SERVICE_UTILS_SERVICE_HELPER_H_
