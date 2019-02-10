#include<gtest/gtest.h>
#include<iostream>
#include<string>
#include<thread>
#include<chrono>
#include<vector>
#include"service_helper.h"
using namespace parser;

TEST(test, init){
 std::string str{};
 std::vector<string> vec{};
 auto str1 = Parser(vec);
 ASSERT_EQ("", str1);
 auto vec1 = Deparser(str1);
 ASSERT_EQ(vec, vec1);
}
TEST(test, add){
  std::string add1("n12hd1oiuh!!");
  std::vector<string> vec{"test1^*(())", "test2h1ue91**~~~!!!", "HJVYUFOIUAMOIQ*!&((*&)Y*)"};
  auto str1 = Parser(vec);
  auto vec1 = Deparser(str1);
  ASSERT_EQ(vec, vec1);
}
using namespace helper;
using namespace std;
IdGenerator idG;
TEST(test, chirpInit){
  auto username = "Adam";
  auto text = "Hello World";
  auto pair = idG();
  auto pid = "-1";
  auto str = chirpInit(username, text, pair.second, pid, pair.first);
  Chirp chirp;
  chirp.ParseFromString(str);
  ASSERT_EQ(username, chirp.username());
  ASSERT_EQ(text, chirp.text());
  ASSERT_EQ(pid, chirp.parent_id());
  ASSERT_EQ(pair.second, chirp.id());
  Chirp* chirp2 = StringToChirp(str);
  ASSERT_EQ(chirp2->username(), chirp.username());
  ASSERT_EQ(chirp2->text(), chirp.text());
  ASSERT_EQ(chirp2->parent_id(), chirp.parent_id());
  ASSERT_EQ(chirp2->id(), chirp.id());
  
}
TEST(test, ID_GEN){
  auto pair =  idG();
  cout << pair.second << endl;
  auto pair4 = idG();
  cout << pair4.second << endl;
  auto pair3 = idG();
  cout << pair3.second << endl;
  auto pair2 =  idG();
  cout << pair2.second << endl;
}
TEST(test, signal) {
  using namespace std::chrono_literals;
  int x = 0;
  std::thread thread(test_x, std::ref(x));
  while("true"){
    std::this_thread::sleep_for(2s);
    cout << "c++ thread and signal test" << endl;
    cout << x << endl;
  }
}
int main(int argc, char* argv[]){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
