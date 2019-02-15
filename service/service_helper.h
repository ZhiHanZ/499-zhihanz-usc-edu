#ifndef SERVICE_HELPER_H_
#define SERVICE_HELPER_H_

#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <atomic>
#include <chrono>
#include <csignal>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "service.pb.h"
#include "unique_id.h"

using namespace std::chrono;
using namespace std;
using chirp::Chirp;
using chirp::Timestamp;
using namespace google::protobuf;
using namespace Id;

namespace helper {
// exit through signals like SIGINT
void signal_handler(int signal_num) { exit(signal_num); }
// convert username, text, id, parent_id to chirp
std::string chirpInit(const std::string& username, const std::string& text,
                      const std::string& id, const std::string& pid,
                      Timestamp time) {
  std::string chirpstring;
  {
    auto chirp = new Chirp; //using unique_ptr in here produce bugs!
    chirp->set_username(username);
    chirp->set_text(text);
    chirp->set_id(id);
    chirp->set_parent_id(pid);
    chirp->set_allocated_timestamp(&time);
    chirp->SerializeToString(&chirpstring);
  }
  return chirpstring;
}
// Convert string to chirp pointer
Chirp* StringToChirp(const std::string& chirpstring) {
  auto chirp = new Chirp;
  chirp->ParseFromString(chirpstring);
  return chirp;
}
}  // namespace helper
namespace parser {
using namespace std;
template <typename T>
// parsing a vector into a string delimiter is "\n"
T Parser(const vector<T>& vecs) {
  string parse{};
  for (const auto& vec : vecs) {
    parse += vec;
    parse += "\n";
  }
  return parse;
}
template <typename T>
// Deparse a strint into a vector
auto Deparser(const T& string) {
  std::istringstream ss(string);
  std::string token;
  std::string delimiter{"\n"};
  vector<T> vec;
  while (std::getline(ss, token, '\n')) {
    vec.push_back(token);
  }
  return vec;
}
}  // namespace parser
#endif
