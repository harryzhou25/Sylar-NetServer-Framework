#include "server/TCPserver.h"
#include "log/logger.h"

static void test() { std::cout << "hello";}

namespace sylar {

static Logger::Ptr g_logger = Name_Logger("system");

TcpServer::TcpServer(EventPoller* ioWorker, EventPoller* accept_worker) 
                :m_ioWorker(ioWorker),
                m_acceptWorker(accept_worker),
                m_recvTimeout((uint64_t)(60 * 1000 * 2)),
                m_name("test"),
                m_running(false){}

TcpServer::~TcpServer() {
    for(auto& i : m_socks) {
        i->close();
    }
    m_socks.clear();
    Log_Debug(g_logger) << "~TcpServer::TcpServer " << m_name;
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
        // Log_Debug(g_logger) << "scheduled startListen";
        m_acceptWorker->schedule([self, sock]() {
            self->startListen(sock);
        });
    }
}

void TcpServer::stop() {
    m_running = false;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this, self]() {
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
        // Log_Debug(g_logger) << "TcpServer::startListen received:" << client->toString();
        if(client) {
            // Log_Debug(g_logger) << "TcpServer::startListen accepted:" << client->toString();
            client->setRecvTimeout(m_recvTimeout);
            m_ioWorker->schedule([self, client]() {
                self->handleClient(client);
            });
            // Log_Debug(g_logger) << "TcpServer::startListen scheduled:" << client->toString();
        }
        else {
            continue;
        }
    }
    // Log_Debug(g_logger) << "TcpServer::startListen quit";
}

void TcpServer::handleClient(Socket::Ptr client) {
    Log_Debug(g_logger) << "TcpServer::handleClient " << client->toString();
}

} // namespace sylar
