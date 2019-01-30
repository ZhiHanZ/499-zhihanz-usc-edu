#include"cchash.h"
#include<gtest/gtest.h>
#include<iostream>

#include<string>
ConcurrentHashTable<std::string, std::string> hs;
TEST(test, init){
	ASSERT_EQ(hs.GetValue("test2", "undefined"), "undefined");
}
TEST(test, add){
	hs.AddOrUpdate("test1", "test1");
	ASSERT_EQ(hs.GetValue("test1", "undefined"), "test1");
	ASSERT_EQ(hs.GetValue("test2", "undefined"), "undefined");
	hs.AddOrUpdate("test2", "test2");
	ASSERT_EQ(hs.GetValue("test2", "undefined"), "test2");
	hs.Add("add1", "add");
	ASSERT_EQ(hs.GetValue("add1"), "add");
}
TEST(test, update){
	hs.AddOrUpdate("test3"," 3 ");
	hs.AddOrUpdate("test3"," 4 ");
	hs.AddOrUpdate("test3"," 5 ");
	hs.AddOrUpdate("test3"," 6 ");
	ASSERT_EQ(hs.GetValue("test3", "undefined"), " 6 ");
}
TEST(test, deletes){
	ASSERT_EQ(hs.GetValue("test2", "undefined"), "test2");
	hs.DeleteKey("test2");
	ASSERT_EQ(hs.GetValue("test2", "undefined"), "undefined");
}
TEST(test, has){
	ASSERT_EQ(hs.Has("test2"), false);
	ASSERT_EQ(hs.Has("test3"), true);
	ASSERT_EQ(hs.Has("test4"), false);
}
int main(int argc, char* argv[]){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
