#ifndef _SYLAR_TCPSERVER_H_
#define _SYLAR_TCPSERVER_H_

#include <memory>
#include <vector>
#include <string.h>
#include <functional>

#include "util/util.h"
#include "socket/socket.h"
#include "eventpoller/eventpoller.h"

namespace sylar {

class TcpServer : public std::enable_shared_from_this<TcpServer>, noncopyable{
public:
    using Ptr = std::shared_ptr<TcpServer>;

    TcpServer(EventPoller* listener = EventPoller::getThis(),
            EventPoller* accept_worker = EventPoller::getThis());

    virtual ~TcpServer();

    virtual bool bind(Address::Ptr addr, bool ssl = false);

    virtual bool bind(const std::vector<Address::Ptr>& addrs,
                    std::vector<Address::Ptr>& fails,
                    bool ssl = false);

    virtual bool start();

    virtual void stop();
    
    bool isStop() const {return !m_running;}

    std::string getName() const {return m_name;}


    uint64_t getRecvTimeout() const {return m_recvTimeout;}

    void setRecvTimeout(const uint64_t timeout) {m_recvTimeout = timeout;}

    std::vector<Socket::Ptr> getSocks() const { return m_socks;}

    virtual void setName(const std::string& name) { m_name = name;}

    virtual void getname() {std::cout << "tcp server\n";}

    virtual void handleClient(Socket::Ptr client);
protected:
    void startListen(Socket::Ptr sock);
protected:
    bool m_running;
    std::string m_name;
    uint64_t m_recvTimeout;
    std::vector<Socket::Ptr> m_socks;
    EventPoller* m_worker;
    EventPoller* m_ioWorker;  
    EventPoller* m_acceptWorker;
};

} // namespace sylar


#endif //_SYLAR_TCPSERVER_H_