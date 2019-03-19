#ifndef UNIQUE_ID_H_
#define UNIQUE_ID_H_
#include "service.pb.h"
#include <atomic>
#include <chrono>
#include <mutex>
#include <string>

using chirp::Timestamp;
using namespace std::chrono;
using namespace std;

namespace Id {
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
class IdGenerator {
public:
  IdGenerator() : sequence_(0) {}
  // Get a Unique id using the machine code of given server(optional)
  string GetId(int64_t machine_code) {
    lock_guard<mutex> lock(id_mutex);
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
  auto operator()(const int64_t &machine_code = 300) {
    time.set_seconds(GetSec());
    time.set_useconds(GetMicroSec());
    return make_pair(time, GetId(machine_code));
  }

private:
  int64_t sequence_ = 0;
  Timestamp time;
  mutex id_mutex;
};
} // namespace Id
#endif // UNIQUE_ID_H_
