#include "server/TCPserver.h"
#include "log/logger.h"

static void test() { std::cout << "hello";}

namespace sylar {

static Logger::Ptr g_logger = Name_Logger("system");

TcpServer::TcpServer(EventPoller* worker, EventPoller* listener) 
                : m_worker(worker),
                m_listener(listener),
                m_recvTimeout((uint64_t)(60 * 1000 * 2)),
                m_name("test"),
                m_running(false){}

TcpServer::~TcpServer() {
    for(auto& i : m_socks) {
        i->close();
    }
    m_socks.clear();
}

bool TcpServer::bind(Address::Ptr addr, bool ssl) {
    std::vector<Address::Ptr> addrs;
    std::vector<Address::Ptr> fails;
    addrs.push_back(addr);
    return bind(addrs, fails, ssl);
}

bool TcpServer::bind(const std::vector<Address::Ptr>& addrs,
                    std::vector<Address::Ptr>& fails,
                    bool ssl) {
    bool failFlag = false;
    for(auto& addr: addrs) {
        Socket::Ptr sock = Socket::CreateTCP(addr);
        if(!sock->bind(addr)) {
            Log_Error(g_logger) << "bind fail errno="
            << errno << " errstr=" << strerror(errno)
            << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            failFlag = true;
            continue;
        }
        if(!sock->listen()) {
            Log_Error(g_logger) << "listen fail errno="
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

    for(auto& i : m_socks) {
        Log_Info(g_logger) << "binded :" << i->toString();
    }

    return true;
}

bool TcpServer::start() {
    if(m_running) {
        return true;
    }
    m_running = true;
    auto self = shared_from_this();
    for(auto sock : m_socks) {
        Log_Debug(g_logger) << "scheduled startListen";
        m_listener->schedule([self, sock]() {
            self->startListen(sock);
        });
    }
}

void TcpServer::stop() {
    m_running = false;
    auto self = shared_from_this();
    m_listener->schedule([this, self]() {
        for(auto& sock : m_socks) {
            sock->cancelAll();
            sock->close();
        }
        m_socks.clear();
    });
}

void TcpServer::startListen(Socket::Ptr sock) {
    auto self = shared_from_this();
    while(m_running) {
        Socket::Ptr client = sock->accept();
        if(client) {
            client->setRecvTimeout(m_recvTimeout);
            m_worker->schedule([self, client]() {
                self->handleClient(client);
            });
        }
        else {
            continue;
        }
    }
}

void TcpServer::handleClient(Socket::Ptr client) {
    getname();
}

} // namespace sylar
