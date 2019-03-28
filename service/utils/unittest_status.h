#ifndef SERVICE_UTILS_UNITTEST_STATUS_H_
#define SERVICE_UTILS_UNITTEST_STATUS_H_
#include <future>
#include <string>
#include <thread>
using std::thread;
namespace unittest {
enum FakeCode { ALREADY_EXISTS, NOT_FOUND, OK };
}
#endif  //  SERVICE_UTILS_UNITTEST_STATUS_H_
