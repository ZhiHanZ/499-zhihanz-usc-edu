#include "cchash.h"
#include<string>

using namespace std;

//helper methods used in key value store
namespace helper {

	int64_t Put(ConcurrentHashTable<string, string>& table
			        , const string& key, const string& value){
		if(table.Has(key)){
			return -1;
		}	else {
			table.Add(key, value);
			return 0;
		}
	}	

	string Get(ConcurrentHashTable<string, string>& table
					  , const string& key){
		return table.GetValue(key);
	}

	int64_t Delete(ConcurrentHashTable<string, string>& table
			          , const string& key){
		if(table.Has(key)){
			table.DeleteKey(key);
			return 0;
		}else {
			return -1;
		}
	}
}
