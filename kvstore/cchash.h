#ifndef CC_HASH_H_
#define CC_HASH_H_
#include <functional>
#include <list>
#include <memory>
#include <shared_mutex>
#include <algorithm>
#include <vector>

template<typename Key, typename Value, typename Hash=std::hash<Key>>
class ConcurrentHashTable {
  public: 
    //Consstruct a hash table with its default size is 19.
    explicit ConcurrentHashTable(unsigned num_buckets = default_buckets_)
      :table_(num_buckets){
      for (unsigned i = 0; i < num_buckets; ++i) {
        table_[i].reset(new BucketType);
      }
    }
    //Get value corresponding to the key;
    Value GetValue(const Key& key){
      return GetBucket(key).GetValue(key);
    }
    //Get value corresponding to the key, if there is no value, return default value(second param).
    Value GetValue(const Key& key, const Value& val){
      return  GetBucket(key).GetValue(key, val);
    }
    //Add key value pair. if keys exists return false
    bool Add(const Key& key, const Value& val){
      return GetBucket(key).AddValue(key, val);
    }
    //check whether table has key
    bool Has(const Key& key){
      return GetBucket(key).HasKey(key);
    }
    //Add or Update key value pair
    void AddOrUpdate(const Key& key, const Value& val) {
      GetBucket(key).AddOrUpdateValue(key, val);
    }
    //delete key.
    void DeleteKey(const Key& key) {
      GetBucket(key).DeleteKey(key);
    }
  private:
   //do not allow copy operation
    ConcurrentHashTable(const ConcurrentHashTable& other) = delete;
    auto& operator= (const ConcurrentHashTable& other) = delete;
    class BucketType
    {
       public:
         bool HasKey(const Key& key){
           std::shared_lock<std::shared_mutex> lock(mutex);
           return (FindEntry(key) == data_.end())? false: true;
         }		
			 //pre: Has Key in hash table
         Value GetValue(const Key& key){
           std::shared_lock<std::shared_mutex> lock(mutex);
           return FindEntry(key)->second;
         }
         Value GetValue(const Key& key, const Key& default_val){
           std::shared_lock<std::shared_mutex> lock(mutex);
           return (FindEntry(key) == data_.end())? default_val: FindEntry(key)->second;
         }
         bool AddValue(const Key& key, const Value& value) {
           std::unique_lock<std::shared_mutex> lock(mutex);
           bucket_iter iter_ = FindEntry(key);
           if (iter_ == data_.end()) {
             data_.push_back(make_pair(key, value));
             return true;
           } 
           else {
             return false;
           }
         }
         void AddOrUpdateValue(const Key& key, const Value& value){
           std::unique_lock<std::shared_mutex> lock(mutex);
           bucket_iter iter_ = FindEntry(key);
           if (iter_ != data_.end()) {
             iter_->second = value;
           } else {
             data_.push_back(make_pair(key, value));
           }
         }
         void DeleteKey(const Key& key) {
           std::unique_lock<std::shared_mutex> lock(mutex);
           if (FindEntry(key) != data_.end()) {
             data_.erase(FindEntry(key));
           }
         }
       private:
         typedef std::pair<Key, Value> bucket_value;
         typedef std::list<bucket_value> bucket_data;
         bucket_data data_;
         typedef typename bucket_data::iterator bucket_iter;
         mutable std::shared_mutex mutex; //c++ 17 feature
         bucket_iter FindEntry(const Key& key){
           return std::find_if(data_.begin(), data_.end(), [&key](bucket_value \
                   const &item) {return item.first == key;});
         }
    };
    std::vector<std::unique_ptr<BucketType>> table_;
    Hash hash_;
    const static unsigned default_buckets_ = 19;
    BucketType& GetBucket(const Key& key) const {
    std::size_t const index_ = hash_(key)%table_.size();
      return *table_[index_];
    }
};
#endif //CC_HASH_H_

