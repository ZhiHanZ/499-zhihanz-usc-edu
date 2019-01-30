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
    ConcurrentHashTable(unsigned int num_buckets = default_buckets_)
			:table_(num_buckets){
      for(unsigned i = 0; i < num_buckets; ++i){
	table_[i].reset(new bucket_type);
      }
    }
    
    Value GetValue(const Key& key){
      return GetBucket(key).GetValue(key);
    }
    Value GetValue(const Key& key, const Value& val){
      return  GetBucket(key).GetValue(key, val);
    }
    bool Add(const Key& key, const Value& val){
      return GetBucket(key).AddValue(key, val);
    }
    bool Has(const Key& key){
      return GetBucket(key).HasKey(key);
    }
    void AddOrUpdate(const Key& key, const Value& val) {
      GetBucket(key).AddOrUpdateValue(key, val);
    }
    void DeleteKey(const Key& key) {
      GetBucket(key).DeleteKey(key);
    }
  private:
   //do not allow copy operation
    ConcurrentHashTable(const ConcurrentHashTable& other) = delete;
    auto& operator= (const ConcurrentHashTable& other) = delete;
    class bucket_type
    {
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
     };
     std::vector<std::unique_ptr<bucket_type>> table_;
     Hash hash_;
     int64_t default_buckets_ = 19;
     bucket_type& GetBucket(const Key& key) const {
       std::size_t const index_ = hash_(key)%table_.size();
       return *table_[index_];
     }
};
#endif //CC_HASH_H_

