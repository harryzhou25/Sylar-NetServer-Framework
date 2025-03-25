#include "server/TCPserver.h"
#include "socket/bytearray.h"
#include "socket/address.h"
#include "socket/socket.h"
#include "eventpoller/eventpoller.h"
#include "rpc/echo_msg.pb.h"
#include "rpc/header.pb.h"
#include <string>
const char* ip = nullptr;
uint16_t port = 8080;

int main(int argc, char** argv) {
    if(argc != 3) {
        std::cout << "Usage [target ip] [target port]\n";
        return -1;
    }

    ip = argv[1];
    port = atoi(argv[2]);

    sylar::IPAddress::Ptr local_addr = sylar::Address::LookupAnyIPAddress(ip);
    // local_addr->setPort(port);

    sylar::IPAddress::Ptr server_addr = sylar::Address::LookupAnyIPAddress(ip);
    server_addr->setPort(port);

    auto sock = sylar::Socket::CreateTCP(local_addr);

    if(!sock->connect(server_addr)) {
        std::cout << "failed to connect\n";
        return 0;
    }

    sylar_rpc::Echo_message msg;
    sylar_rpc::Header* header = msg.mutable_header();

    std::string service_name = "echo";
    std::string method_name = "echo repeat";

    header->set_service_name("echo");
    header->set_method_name("echo repeat");
    header->set_args_size(1);
    // msg.set_allocated_header(&header);
    msg.set_echo_message("hello rpc");

    std::string out = msg.SerializeAsString();

    // google::protobuf::ShutdownProtobufLibrary();
    sock->send(out.c_str(), out.length());
    return 0;
}
