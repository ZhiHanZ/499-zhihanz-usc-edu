#include "kvhelper.h"
namespace key_val {
namespace helper {
// Put given key and value to table, And if key exists, update value
bool Put(ConcurrentHashTable<string, string> &table, const string &key,
         const string &value) {
  table.AddOrUpdate(key, value);
  return 0;
}
string Get(const ConcurrentHashTable<string, string> &table,
           const string &key) {
  return table.GetValue(key);
}
// Delete value through key
// 0 represents delete successful
// -1 represents delete a key do not exists

int64_t Delete(ConcurrentHashTable<string, string> &table, const string &key) {
  if (table.Has(key)) {
    table.DeleteKey(key);
    return 0;
  }
  return -1;
}
}  // namespace helper
}  // namespace key_val
