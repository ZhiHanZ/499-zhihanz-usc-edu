#ifndef SERVICE_HELPER_H_
#define SERVICE_HELPER_H_

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <google/protobuf/message.h>
#include "service.pb.h"
#include <grpcpp/grpcpp.h>
#include <mutex>
#include <string>
#include <chrono>
#include <atomic>
#include <csignal>
#include <thread>

using namespace std::chrono;
using namespace std;
using chirp::Chirp;
using chirp::Timestamp;
using namespace google::protobuf;

namespace helper {
  void signal_handler( int signal_num ){
    exit(signal_num);
  }
  void test_x(int& x){
    signal(SIGINT, signal_handler);
    using namespace std::chrono_literals;
    while (true){
      std::this_thread::sleep_for(100ms);
      x++;
    }
  }
  //convert username, text, id, parent_id to chirp
  std::string chirpInit(const std::string& username,
                  const std::string& text,
                  const std::string& id,
                  const std::string& pid,
                  Timestamp time){
    std::string chirpstring;
    {
      //How to use make_unique in here?
      auto chirp = new Chirp;
      chirp->set_username(username);
      chirp->set_text(text);
      chirp->set_id(id);
      chirp->set_parent_id(pid);
      chirp->set_allocated_timestamp(&time);
      chirp->SerializeToString(&chirpstring);
    }
    return chirpstring;
  }
  //Convert string to chirp pointer
  Chirp* StringToChirp(const std::string& chirpstring){
    auto chirp = new Chirp;
    chirp->ParseFromString(chirpstring);
    return chirp;
  }
  //Get current micro second 
  static int64_t GetMicroSec(){
    return duration_cast<microseconds> (system_clock::now().\
                    time_since_epoch()).count();
	}
  //Get current second
  static int64_t GetSec() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return duration_cast<seconds>(now).count();
	}
  //This is a unique id generator 
  class  IdGenerator{
    public:
      IdGenerator(): sequence_(0){}
      string GetId(int64_t machine_code){
          lock_guard<mutex> lock(id_mutex);
          int64_t value = 0;
          auto time = GetSec();
          value |= time << 22;
          value |= machine_code & 0x3FF << 12;
          value |= sequence_++ &0xFFF;
          if (sequence_ == 0x1000) {
            sequence_ = 0;
          }
          return std::to_string(value);
        }
      //functor which can accept a machine code and return 
      //a std::pair<time, id>
      auto operator()(const int64_t& machine_code = 300) {
        time.set_seconds(GetSec());
        time.set_useconds(GetMicroSec());
        return  make_pair(time,GetId(machine_code));
      }
    private:
      Timestamp time;
      int64_t sequence_ = 0;
      mutex id_mutex;
	};
}
namespace parser{
  using namespace std;
  template<typename T>
   //parsing a vector into a string delimiter is "\n"
  T Parser(const vector<T>& vecs){
    string parse{};
    for(const auto& vec: vecs){
      parse += vec;  
      parse += "\n";
    }
    return parse;
  }
  template<typename T>
  //Deparse a strint into a vector
  auto Deparser(const T& string){
    std::istringstream ss(string);
    std::string token;
    std::string delimiter {"\n"};
    vector<T> vec;
    while(std::getline(ss, token, '\n')){
      vec.push_back(token);
    }
    return vec;
  }
}
#endif
