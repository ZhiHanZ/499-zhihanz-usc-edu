CXX = clang++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++17
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
					 -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
					 -ldl -lgtest -lpthread 
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = utils/pb/
vpath %.proto $(PROTOS_PATH)
all: service unittest_service
service: service.o utils/unique_id.o utils/kvstore_client.o utils/service_helper.o utils/pb/service.pb.o utils/pb/service.grpc.pb.o utils/pb/kvstore.pb.o utils/pb/kvstore.grpc.pb.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_service: service_string.o utils/kvstore_string.o utils/service_helper.o utils/pb/service.pb.o utils/service_helper.o utils/unique_id.o unittest_service.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
.PRECIOUS: %.grpc.pb.cc
service.grpc.pb.cc: utils/pb/service.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=utils/pb/. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
kvstore.grpc.pb.cc: utils/pb/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=utils/pb/. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
.PRECIOUS: %.pb.cc
kvstore.pb.cc: utils/pb/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=utils/pb/. $<
service.pb.cc: utils/pb/service.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=utils/pb/. $<
clean:
	rm *.o unittest_service service
