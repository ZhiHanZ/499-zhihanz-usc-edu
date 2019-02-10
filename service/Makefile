CXX = clang++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++17
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
					 -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
					 -ldl -lgtest -lpthread 
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = ../protoc
vpath %.proto $(PROTOS_PATH)
all: unittest_service_helper unittest_service service
service: service.o service.pb.o service.grpc.pb.o kvstore.pb.o kvstore.grpc.pb.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_unique_id: unittest_unique_id.cc
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_service_helper: unittest_service_helper.o service.pb.o service.grpc.pb.o kvstore.pb.o kvstore.grpc.pb.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_service: unittest_service.o service.pb.o service.grpc.pb.o kvstore.pb.o kvstore.grpc.pb.o
	$(CXX) $^ $(LDFLAGS) -g -o $@
unittest_service.o: unittest_service.cc
	$(CXX) -c -g -std=c++17 unittest_service.cc
client: kvstore.pb.o kvstore.grpc.pb.o client.o
	$(CXX) $^ $(LDFLAGS) -o $@
.PRECIOUS: %.grpc.pb.cc
service.grpc.pb.cc: ../protoc/service.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
kvstore.grpc.pb.cc: ../protoc/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<
.PRECIOUS: %.pb.cc
kvstore.pb.cc: ../protoc/kvstore.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<
service.pb.cc: ../protoc/service.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<
clean:
	rm *.o
