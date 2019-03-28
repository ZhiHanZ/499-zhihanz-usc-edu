#ifndef SERVICE_UNIQUE_ID_H_
#define SERVICE_UNIQUE_ID_H_
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <utility>
#include "pb/service.pb.h"

using chirp::Timestamp;
using std::lock_guard;
using std::mutex;
using std::string;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
namespace services {
namespace chirp_id {
// Get current micro second
static int64_t GetMicroSec() {
  return duration_cast<microseconds>(system_clock::now().time_since_epoch())
      .count();
}
// Get current second
static int64_t GetSec() {
  auto now = std::chrono::system_clock::now().time_since_epoch();
  return duration_cast<seconds>(now).count();
}
// This is a unique id generator
// Example Usage:
// IdGnerator id_generator;
// auto id_pair = id_generator();
// chirp::Timestamp created_time = id_pair.first;
// std::string created_id = id_pair.second;
// NOTE: You should use dependency injection
// if you want to use it in your class.
class IdGenerator {
 public:
  IdGenerator() : sequence_(0) {}
  // Get a Unique id using the machine code of given server(optional)
  string GetId(int64_t machine_code);
  // functor which can accept a machine code and return
  // a std::pair<time, id>
  auto operator()(const int64_t &machine_code = 300)
      -> std::pair<Timestamp, string>;

 private:
  int64_t sequence_ = 0;
  Timestamp time_;
  mutex id_mutex_;
};
}  // namespace chirp_id
}  // namespace services
#endif  // SERVICE_UNIQUE_ID_H_
