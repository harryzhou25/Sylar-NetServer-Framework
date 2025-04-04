#include "server/TCPserver.h"
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

// static sylar::Logger::Ptr r_logger = Root_Logger();

class EchoServer : public sylar::TcpServer {
public:
    EchoServer();
    void getname() override {std::cout << "echo server\n";}
    void on_watcher(int type, int stat, const std::string& path, sylar::zkClient::ptr client);
    void handleClient(sylar::Socket::Ptr client) override;
    bool bind(const std::vector<sylar::Address::Ptr>& addrs, 
                            std::vector<sylar::Address::Ptr>& fails, 
                            bool ssl = false);
private:
    sylar::zkClient::Ptr m_zk;
};

EchoServer::EchoServer()
    :m_zk(new sylar::zkClient) {
    m_zk->init("127.0.0.1:2181", 3000, std::bind(&EchoServer::on_watcher, this,
                                                std::placeholders::_1,
                                                std::placeholders::_2,
                                                std::placeholders::_3,
                                                std::placeholders::_4));
}

bool EchoServer::bind(const std::vector<sylar::Address::Ptr>& addrs, 
                    std::vector<sylar::Address::Ptr>& fails, 
                    bool ssl) {
    bool failFlag = false;
    for(auto& addr: addrs) {
        sylar::Socket::Ptr sock = sylar::Socket::CreateTCP(addr);
        if(!sock->bind(addr)) {
            std::cout << "bind fail errno="
            << errno << " errstr=" << strerror(errno)
            << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            failFlag = true;
            continue;
        }
        if(!sock->listen()) {
            std::cout << "listen fail errno="
                << errno << " errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            failFlag = true;
            continue;
        }
        m_socks.push_back(sock);
    }
    
    if(failFlag) {
        m_socks.clear();
        return false;
    }

    std::string path = "/echo";
    std::string data_prefix = "/echo_";
    std::string realPathBuffer;
    m_zk->create(path, "", realPathBuffer);
    for(auto& i : m_socks) {
        std::cout << "binded :" << i->toString();
        auto local_addr = i->getLocalAddr();
        std::string data = local_addr->toString();
        m_zk->create(path + data_prefix + data, data, realPathBuffer, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE);
    }
    return true;
}

void EchoServer::on_watcher(int type, int stat, const std::string& path, sylar::zkClient::ptr client) {
    std::cout << "received event type: " << type << ',' << stat << '\n';
    if(stat == ZOO_CONNECTED_STATE) {
        std::cout << "connected server\n";
    }
    if(!path.empty()) {
        std::cout << " path: " << path << '\n';
    }
}

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
        std::string msg_byte = ba->toString();
        sylar_rpc::Echo_message msg;
        if(!msg.ParseFromString(msg_byte)) {
            std::cout << "failed to parse: " << msg_byte << '\n';
        }
        else {
            std::cout << "--- Received echo message ---\n";
            std::cout << "Header: " << msg.header().service_name() << ',' << msg.header().method_name() << ',' << msg.header().args_size() << '\n';
            std::cout << "Message: " << msg.echo_message() << '\n';
        }
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

int main(int argc, char** argv) {
    sylar::EventPoller sch(2);
    sch.schedule(run);
    return 0;
}

// eth0 172.29.252.203:0
// lo 127.0.0.1:0