#include "server/RPCserver.h"
#include "socket/bytearray.h"
#include "socket/address.h"
#include "socket/socket.h"
#include "eventpoller/eventpoller.h"
#include "rpc/echo_msg.pb.h"
#include "rpc/header.pb.h"
#include "zkClient/zkClient.h"

#include<vector>
#include<map>
#include<string>

class EchoServer : public sylar::RPCserver {
public:
    using Ptr = std::shared_ptr<EchoServer>;
    EchoServer() = default;
    EchoServer(std::string addr) : m_addr(addr) {}
    ~EchoServer() = default;
    bool autoRegister() {
        auto self = shared_from_this();
        bool res = true;
        res &= registMethod("echo", m_addr, m_addr, std::bind(&EchoServer::echo, this, std::placeholders::_1));
        return res;
    }
private:
    std::string m_addr;

    void handleClient(sylar::Socket::Ptr client) override {
        sylar::ByteArray::Ptr ba(new sylar::ByteArray);
        while(true) {
            ba->clear();
            std::vector<iovec> buffer;
            ba->getWriteBuffers(buffer, 1024);
    
            int rt = client->recv(&buffer[0], buffer.size());
            if(rt == 0) {
                std::cout << "client close\n";
                break;
            }
            else if(rt < 0) {
                std::cout << "client error rt=" << rt << " errno=" << errno << " errstr=" << strerror(errno);
                break;
            }
            ba->setPosition(ba->getPosition() + rt);
            ba->setPosition(0);
            sylar_rpc::Echo_message msg;
            msg.ParseFromString(ba->toString());
            auto method_name = msg.header().method_name();
            if(m_methods.find(method_name) != m_methods.end()) {
                auto func = m_methods[method_name].get();
                (*func)(ba->toString());
            }
            std::cout.flush();
        }
    }

    void echo(std::string msg_byte) {
        sylar_rpc::Echo_message msg;
        if(!msg.ParseFromString(msg_byte)) {
            std::cout << "failed to parse: " << msg_byte << '\n';
        }
        else {
            std::cout << "Received Message: " << msg.echo_message() << '\n';
        }
    }
};

void run() {
    std::string address = "127.0.0.1:34280";
    EchoServer::Ptr es(new EchoServer(address));
    auto addr = sylar::Address::LookupAny(address);
    while(!es->bind(addr)) {
        sleep(2);
    }
    // es->registMethod("echo", address, address)
    es->start();
    if(!es->autoRegister()) {
        std::cout << "register failed\n";
    }
}

int main(int argc, char** argv) {
    sylar::EventPoller sch(2);
    sch.schedule(run);
    return 0;
}