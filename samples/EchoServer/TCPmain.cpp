#include "server/TCPserver.h"
#include "socket/bytearray.h"
#include "socket/address.h"
#include "socket/socket.h"
#include "eventpoller/eventpoller.h"

#include<vector>
#include<map>

// static sylar::Logger::Ptr r_logger = Root_Logger();

class EchoServer : public sylar::TcpServer {
public:
    EchoServer() = default;
    void getname() override {std::cout << "echo server\n";}
    void handleClient(sylar::Socket::Ptr client) override;
};

void EchoServer::handleClient(sylar::Socket::Ptr client) {
    std::cout << "EchoServer::handleClient()" << client->toString() << '\n';
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
        std::cout << ba->toString() << '\n';
        std::cout.flush();
    }
}

void run() {
    EchoServer::Ptr es(new EchoServer);
    auto addr = sylar::Address::LookupAny("127.0.0.1:34280");
    while(!es->bind(addr)) {
        sleep(2);
    }
    es->start();
}

int main() {
    sylar::EventPoller sch(2);
    sch.schedule(run);
    return 0;
}

// eth0 172.29.252.203:0
// lo 127.0.0.1:0