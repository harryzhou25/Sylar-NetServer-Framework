#include "server/TCPserver.h"
#include "socket/bytearray.h"
#include "socket/address.h"
#include "socket/socket.h"
#include "eventpoller/eventpoller.h"
#include "rpc/echo_msg.pb.h"
#include "rpc/header.pb.h"
#include "zkClient/zkClient.h"
#include <string>
const char* ip = nullptr;
uint16_t port = 8080;

void on_watcher(int type, int stat, const std::string& path, sylar::zkClient::ptr client) {
    std::cout << "received event type: " << type << ',' << stat << '\n';
    if(stat == ZOO_CONNECTED_STATE) {
        std::cout << "connected server\n";
    }
    if(!path.empty()) {
        std::cout << " path: " << path << '\n';
    }
}

int main(int argc, char** argv) {
    // if(argc != 3) {
    //     std::cout << "Usage [target ip] [target port]\n";
    //     return -1;
    // }

    // ip = argv[1];
    // port = atoi(argv[2]);

    sylar::IPAddress::Ptr local_addr = sylar::Address::LookupAnyIPAddress("127.0.0.1");

    sylar::zkClient::Ptr zk_client(new sylar::zkClient);
    
    zk_client->init("127.0.0.1:2181", 5000, on_watcher);
    std::string path = "/echo";
    std::vector<std::string> nodes;
    zk_client->getChildren(path, nodes, 0);

    if(nodes.size() <= 0) return 0;

    auto node = nodes[0];

    std::string remote_addr;
    remote_addr.resize(512);
    zk_client->get(path + '/' + node, remote_addr, false);

    auto pos = remote_addr.find(':');

    std::cout << remote_addr << ' ' << pos << '\n';

    ip = remote_addr.substr(0, pos).c_str();
    port = std::stoi(remote_addr.substr(pos+1));

    std::cout << "get remote ip from zookeeper : " << remote_addr << '\n';

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
    std::string method_name = "echo";

    header->set_service_name(service_name);
    header->set_method_name(method_name);
    header->set_args_size(1);
    // msg.set_allocated_header(&header);
    msg.set_echo_message("hello rpc");

    std::string out = msg.SerializeAsString();

    // google::protobuf::ShutdownProtobufLibrary();
    sock->send(out.c_str(), out.length());
    return 0;
}
