#include "unique_id.h"
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <utility>
#include "pb/service.pb.h"

using chirp::Timestamp;
using std::mutex;
using std::string;
using std::lock_guard;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
namespace services {
namespace chirp_id {
string IdGenerator::GetId(int64_t machine_code = 300){
  // It is a unique id algorithm mimic to Twitter Snow Flake Algorithm
  lock_guard<mutex> lock(id_mutex_);
  int64_t value = 0;
  auto time = GetSec();
  value |= time << 22;
  value |= machine_code & 0x3FF << 12;
  value |= sequence_++ & 0xFFF;
  if (sequence_ == 0x1000) {
    sequence_ = 0;
  }
  return std::to_string(value);
}
  // functor which can accept a machine code and return
  // a std::pair<time, id>
auto IdGenerator::operator()(const int64_t &machine_code) 
                            -> std::pair<Timestamp, std::string> {
    time_.set_seconds(GetSec());
    time_.set_useconds(GetMicroSec());
    return make_pair(time_, GetId(machine_code));
}
}  // namespace chirp_id
}  // namespace service
