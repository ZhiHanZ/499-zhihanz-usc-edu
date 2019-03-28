CXX = clang++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++17
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
					 -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
					 -ldl -lgtest -lpthread 
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = pb/
vpath %.proto $(PROTOS_PATH)
all: kvstore.pb.cc service.grpc.pb.cc kvstore.grpc.pb.cc service.pb.cc unittest_service_helper unittest_service_helper unittest_parser unittest_kvstore
service: service.o service.pb.o service.grpc.pb.o kvstore.pb.o kvstore.grpc.pb.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_parser: unittest_parser.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_unique_id: unittest_unique_id.o unique_id.o pb/service.pb.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_kvstore: unittest_kvstore.o kvstore_string.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_service_helper: unittest_service_helper.o service_helper.o unique_id.o pb/service.pb.o pb/service.grpc.pb.o pb/kvstore.pb.o pb/kvstore.grpc.pb.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_service: service_string.o service.pb.o unittest_service.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
client: kvstore.pb.o kvstore.grpc.pb.o client.o
	$(CXX) $^ $(LDFLAGS) -o $@
.PRECIOUS: %.grpc.pb.cc
service.grpc.pb.cc: pb/service.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=pb/. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
kvstore.grpc.pb.cc: pb/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=pb/. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
.PRECIOUS: %.pb.cc
kvstore.pb.cc: pb/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=pb/. $<
service.pb.cc: pb/service.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=pb/. $<
clean:
	rm *.o
