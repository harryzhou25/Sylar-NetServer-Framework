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

namespace sylar {

class RPCserver : public TcpServer, std::enable_shared_from_this<RPCserver>{
public:
    using Ptr = std::shared_ptr<RPCserver>;
    using FuncType = std::function<void()>;
    using FuncPtr = std::shared_ptr<FuncType>;

private:
    struct Service {
        std::string service_name;
        std::unordered_map<std::string, FuncPtr> method_list;
    };

public:
    RPCserver(EventPoller* worker, EventPoller* listener, const std::string& zk_host);

    ~RPCserver() = default;

    bool addMethod(std::string service, std::string method, FuncType& cb);

    void setTimeout(uint64_t timeout) {m_recvTimeout = timeout;}

    void setName(const std::string& name) {m_name = name;}

    std::string getName() const {return m_name;}
private:
    void onConnect(const std::string& path, zkClient::ptr client);
    void onChanged(const std::string& path, zkClient::ptr client);
    void onDeleted(const std::string& path, zkClient::ptr client);
    void onExpiredSession(const std::string& path, zkClient::ptr client);
    void onWatch(int type, int stat, const std::string& path, zkClient::ptr);

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

    std::shared_mutex m_service_mtx;
    std::unordered_map<std::string, Service> m_services;
};

} // namespace sylar


#endif //_SYLAR_RPC_SERVER_H_