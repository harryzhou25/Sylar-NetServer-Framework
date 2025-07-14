#ifndef _SYLAR_RPC_SERVER_H_
#define _SYLAR_RPC_SERVER_H_

#include <map>
#include <string>
#include <memory>
#include <functional>
#include <shared_mutex>

#include "util/util.h"
#include "zkClient/zkClient.h"
#include "timer/timer.h"
#include "server/TCPserver.h"
#include "socket/bytearray.h"

namespace sylar {

class RPCserver : public TcpServer{
public:
    using Ptr = std::shared_ptr<RPCserver>;
    using FuncType = std::function<void(std::string)>;
    using FuncPtr = std::shared_ptr<FuncType>;
public:
    RPCserver(EventPoller* worker = EventPoller::getThis(), EventPoller* listener = EventPoller::getThis(), 
            const std::string& zk_host = "127.0.0.1:2181");

    bool registMethod(std::string service, std::string method, std::string data, FuncType cb);

    void setTimeout(uint64_t timeout) {m_recvTimeout = timeout;}

    void setName(const std::string& name) {m_name = name;}

    std::string getName() const {return m_name;}
private:
    void onConnect(const std::string& path, zkClient::ptr client);
    void onChanged(const std::string& path, zkClient::ptr client);
    void onDeleted(const std::string& path, zkClient::ptr client);
    void onExpiredSession(const std::string& path, zkClient::ptr client);
    void onWatch(int type, int stat, const std::string& path, zkClient::ptr);
    virtual void handleClient(Socket::Ptr client) override;
private:
    bool m_running;
    semaphore* m_sem;

    std::string m_name;
    std::string m_host;
    std::string m_zk_host;
    uint64_t m_recvTimeout;

    std::vector<Socket::Ptr> m_socks;
    
    Timer::Ptr m_timer;

    EventPoller* m_worker;
    EventPoller* m_listener; 

    zkClient::Ptr m_zkClient;
protected:
    std::shared_mutex m_service_mtx;
    std::unordered_map<std::string, FuncPtr> m_methods;
};

} // namespace sylar


#endif //_SYLAR_RPC_SERVER_H_