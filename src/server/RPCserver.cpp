#include "server/RPCserver.h"
#include "log/logger.h"

#include<string>

namespace sylar {

static sylar::Logger::Ptr g_logger = SYLAR_LOG_NAME("system");

static std::string methodToPath(const std::string& service, const std::string& method) { return service + "/" + method;}

RPCserver::RPCserver(EventPoller* worker, EventPoller* listener, const std::string& zk_host)
                    :m_zk_host(zk_host),
                    TcpServer(worker, listener) {
    m_worker = worker;
    m_listener = listener;
}

bool RPCserver::addMethod(std::string service, std::string method, FuncType& cb) {
    std::string path = methodToPath(service, method);
    auto rt = m_zkClient->exists(path, false);
    if(rt == ZOK) {
        return true;
    }
    std::string new_val(1024, 0);
    rt = m_zkClient->create(path, m_host, new_val);
    if(rt == ZOK) {
        m_services[service].method_list[method] = std::make_shared<FuncType>(cb);
        return true;
    }
    return false;
}

void RPCserver::onWatch(int type, int stat, const std::string& path, zkClient::ptr client) {
    if(stat == zkClient::StateType::CONNECTED) {
        if(type == zkClient::EventType::SESSION) {
            return onConnect(path, client);
        } 
        else if(type == zkClient::EventType::CHANGED) {
            return onChanged(path, client);
        } 
        else if(type == zkClient::EventType::DELETED) {
            return onDeleted(path, client);
        } 
    } else if(stat == zkClient::StateType::EXPIRED_SESSION) {
        if(type == zkClient::EventType::SESSION) {
            return onExpiredSession(path, client);
        }
    }
    Log_Error(g_logger) << "onWatch hosts=" << m_zk_host
        << " type=" << type << " stat=" << stat
        << " path=" << path << " client=" << client;
}

void RPCserver::onConnect(const std::string& path, zkClient::ptr client) {
    Log_Info(g_logger) << "zookeeper connected";
    m_sem->notify();
}

void RPCserver::onChanged(const std::string& path, zkClient::ptr client) {
    Log_Info(g_logger) << "rpc server on changed: " << path;
}

void RPCserver::onDeleted(const std::string& path, zkClient::ptr client) {
    Log_Info(g_logger) << "rpc server on deleted: " << path;
}

void RPCserver::onExpiredSession(const std::string& path, zkClient::ptr client) {
    Log_Info(g_logger) << "rpc server on expired: " << path;
    m_zkClient->reconnect();
}

} // namespace sylar
