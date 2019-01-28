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
all: unittest_cchash
kvstore.grpc.pb.cc: ../protoc/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
service.grpc.pb.cc: ../protoc/service.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
kvstore.pb.cc: ../protoc/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<
service.pb.cc: ../protoc/service.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<
vpath %.proto $(PROTOS_PATH)
unittest_cchash: unittest_cchash.o
	$(CXX) -o unittest_cchash unittest_cchash.o -lpthread -lgtest -lgtest_main
unittest_cchash.o: unittest_cchash.cc cchash.h
	$(CXX) $(CXXFLAGS) -c unittest_cchash.cc
test: kvstore.pb.o kvstore.grpc.pb.o test.o service.grpc.pb.o service.pb.o
	$(CXX) $^ $(LDFLAGS) -o $@
client: kvstore.pb.o kvstore.grpc.pb.o client.o service.pb.o
	$(CXX) $^ $(LDFLAGS) -o $@
kvstore: kvstore.pb.o kvstore.grpc.pb.o kvstore.o
	$(CXX) $^ $(LDFLAGS) -o $@
kvstore.o: kvstore.cc
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $^
.PRECIOUS: %.grpc.pb.cc
clean:
	rm *.o unittest_cchash
