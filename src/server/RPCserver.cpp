#include "server/RPCserver.h"
#include "log/logger.h"

#include<string>


namespace sylar {

static Logger::Ptr g_logger = Name_Logger("system");

static std::string methodToPath(const std::string& service, const std::string& method) { return "/" + service + "/" + method;}

RPCserver::RPCserver(EventPoller* worker, EventPoller* listener, const std::string& zk_host)
                    :m_zk_host(zk_host), m_zkClient(new sylar::zkClient), TcpServer(worker, listener) {
    m_worker = worker;
    m_listener = listener;
    m_zkClient->init(m_zk_host, 3000, std::bind(&RPCserver::onWatch, this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));
}

bool RPCserver::registMethod(std::string service, std::string method, std::string data, FuncType cb) {
    std::string path = methodToPath(service, method);
    Log_Debug(g_logger) << "method path: " << path << ',' << service << ',' << method;
    auto rt = m_zkClient->exists(path, false);
    if(rt == ZOK) {
        return true;
    }
    std::string new_val(1024, 0);
    m_zkClient->create("/" + service, "", new_val);
    rt = m_zkClient->create(path, data, new_val, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE);
    if(rt == ZOK) {
        m_methods[service] = std::make_shared<FuncType>(cb);
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
    // m_sem->notify();
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

void RPCserver::handleClient(Socket::Ptr client) {
    Log_Debug(g_logger) << "recevied client " << client->toString();
}

} // namespace sylar
