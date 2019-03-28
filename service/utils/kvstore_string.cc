#include "kvstore_string.h"
#include <string>
using std::string;
namespace unittest {

bool UnitTestKVClient::PutOrUpdate(const std::string &key,
                                   const std::string &value) {
  table_.AddOrUpdate(key, value);
  return true;
}
bool UnitTestKVClient::Put(const std::string &key, const std::string &value) {
  if (table_.Has(key)) return false;
  table_.Add(key, value);
  return true;
}
// Get the value of corresponding key
string UnitTestKVClient::GetValue(const std::string &key) const {
  string response = table_.GetValue(key);
  return response;
}
// return whether key value store has such key
bool UnitTestKVClient::Has(const std::string &key) const {
  return table_.Has(key);
}
// delete corresponding key value pair through key
bool UnitTestKVClient::Delete(std::string key) {
  table_.DeleteKey(key);
  return true;
}
}  // namespace unittest
