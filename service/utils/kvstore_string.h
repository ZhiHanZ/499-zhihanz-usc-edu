#ifndef SERVICE_KVSTORE_STRING_H_
#define SERVICE_KVSTORE_STRING_H_
#include <string>
#include "cchash.h"
using std::string;
using utils::ConcurrentHashTable;
namespace unittest {
// The kvstore client which is used for unittest
// Support Put, PutOrUpdate, GetValue through key, Delete key value pair through
// key operations
class UnitTestKVClient final {
 public:
  UnitTestKVClient() : table_{} {}
  // Put or update the key value pair in key value store
  bool PutOrUpdate(const std::string &key, const std::string &value);
  // Put the key value pair into key value store
  // If store has given key return false
  bool Put(const std::string &key, const std::string &value);
  // Get the value of corresponding key
  string GetValue(const std::string &key) const;
  // return whether key value store has such key
  bool Has(const std::string &key) const;
  // delete corresponding key value pair through key
  bool Delete(std::string key);

 private:
  ConcurrentHashTable<string, string>
      table_;  // Hash Table injected for the purpose of test
};
}  // namespace unittest

#endif  // SERVICE_KVSTORE_STRING_H_
