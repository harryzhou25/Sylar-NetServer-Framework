#include "zkClient/zkClient.h"
#include "socket/address.h"
#include "socket/socket.h"
#include <iostream>
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
    if(argc != 3) {
        std::cout << "Usage [local ip] [local port]\n";
        return -1;
    }

    std::string ip_str(argv[1]);
    std::string port_str(argv[2]);

    ip = ip_str.c_str();
    port = atoi(port_str.c_str());

    std::cout << ip_str << ' ' << port_str << '\n';
    std::cout << ip << ' ' << port << '\n';

    sylar::IPAddress::Ptr local_addr = sylar::Address::LookupAnyIPAddress(ip);
    local_addr->setPort(port);

    auto sock = sylar::Socket::CreateTCP(local_addr);
    std::string path = "/echo";
    std::string data = ip_str + ':' + port_str;
    std::string realPathBuffer;
    int flags = ZOO_EPHEMERAL;

    sylar::zkClient::Ptr zk(new sylar::zkClient);
    std::cout << zk->init("127.0.0.1:2181", 3000, on_watcher) << '\n';
    usleep(100 * 1000);
    auto state = zk->create(path, "", realPathBuffer);
    if(ZOK == state) {
        std::cout << "Echo Node created\n";
    }
    else if(ZNODEEXISTS == state){
        std::cout << "Echo Node already exists: " << state << '\n';
    }
    else {
        std::cout << "Erro occur during service registeration: " << state << '\n';
    }
    state = zk->create(path + "/echo_" + data, data, realPathBuffer, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE);
    if(ZOK == state) {
        std::cout << "service registered " << data << '\n';
    }
    else if(ZNODEEXISTS == state){
        std::cout << "Node already exists: " << state << '\n';
    }
    else {
        std::cout << "Erro occur during service registeration: " << state << '\n';
    }
    while(true) {

    }
    return 0;
}