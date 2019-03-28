CXX = clang++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++17
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
					 -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
					 -ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = utils/pb/
all: kvstore.pb.cc kvstore.grpc.pb.cc kvstore
vpath %.proto $(PROTOS_PATH)
unittest_kvhelper: unittest_kvhelper.o
	$(CXX) -o unittest_kvhelper unittest_kvhelper.o -lpthread -lgtest
unittest_kvhelper.o: unittest_kvhelper.cc
	$(CXX) $(CXXFLAGS) -c unittest_kvhelper.cc
unittest_cchash: unittest_cchash.o
	$(CXX) -o unittest_cchash unittest_cchash.o -lpthread -lgtest -lgtest_main
unittest_cchash.o: unittest_cchash.cc cchash.h
	$(CXX) $(CXXFLAGS) -c unittest_cchash.cc
test: kvstore.pb.o kvstore.grpc.pb.o test.o service.grpc.pb.o service.pb.o
	$(CXX) $^ $(LDFLAGS) -o $@
client: kvstore.pb.o kvstore.grpc.pb.o client.o service.pb.o
	$(CXX) $^ $(LDFLAGS) -o $@
kvstore: utils/kvhelper.o utils/pb/kvstore.pb.o utils/pb/kvstore.grpc.pb.o kvstore.o
	$(CXX) $^ $(LDFLAGS) -o $@
kvstore.o: kvstore.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $^
.PRECIOUS: %.grpc.pb.cc
kvstore.grpc.pb.cc: utils/pb/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=utils/pb/. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
.PRECIOUS: %.pb.cc
kvstore.pb.cc: utils/pb/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=utils/pb/. $<
clean:
	rm *.o unittest_cchash unittest_kvhelper *.pb.cc
