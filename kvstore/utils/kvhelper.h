#ifndef KVSTORE_UTILS_KVHELPER_H_
#define KVSTORE_UTILS_KVHELPER_H_
#include <string>
#include "cchash.h"

using std::string;
using utils::ConcurrentHashTable;

// helper methods used in key value store
namespace key_val {
namespace helper {
// Put given key and value to table, And if key exists, update value
bool Put(ConcurrentHashTable<string, string> &table, const string &key,
         const string &value);
// Get value through key
string Get(const ConcurrentHashTable<string, string> &table, const string &key);
// Delete value through key
// 0 represents delete successful
// -1 represents delete a key do not exists

int64_t Delete(ConcurrentHashTable<string, string> &table, const string &key);
}  //  namespace helper
}  //  namespace key_val
#endif  // KVSTORE_UTILS_KVHELPER_H_
