CXX = clang++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++17
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
					 -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
					 -ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = ../protoc
all: unittest_cchash unittest_kvhelper
unittest_kvhelper: unittest_kvhelper.o kvhelper.o
	$(CXX) -o unittest_kvhelper unittest_kvhelper.o kvhelper.o -lpthread -lgtest
unittest_kvhelper.o: unittest_kvhelper.cc
	$(CXX) $(CXXFLAGS) -c unittest_kvhelper.cc
unittest_cchash: unittest_cchash.o
	$(CXX) -o unittest_cchash unittest_cchash.o -lpthread -lgtest -lgtest_main
unittest_cchash.o: unittest_cchash.cc cchash.h
	$(CXX) $(CXXFLAGS) -c unittest_cchash.cc
clean:
	rm *.o unittest_cchash unittest_kvhelper
