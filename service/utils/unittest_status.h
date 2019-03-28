#ifndef UTIL_UNITTEST_STATUS_H_
#define UTIL_UNITTEST_STATUS_H_
#include <string>
#include <thread>
#include <future>
using std::thread;
namespace unittest{
enum FakeCode {
  ALREADY_EXISTS,
  NOT_FOUND,
  OK
};
}
#endif  //  UTIL_UNITTEST_STATUS_H_
