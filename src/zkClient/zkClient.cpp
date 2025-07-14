#include "zkClient/zkClient.h"

namespace sylar {

//static const int CREATED = ZOO_CREATED_EVENT;
//static const int DELETED = ZOO_DELETED_EVENT;
//static const int CHANGED = ZOO_CHANGED_EVENT;
//static const int CHILD   = ZOO_CHILD_EVENT;
//static const int SESSION = ZOO_SESSION_EVENT;
//static const int NOWATCHING = ZOO_NOTWATCHING_EVENT;

const int zkClient::EventType::CREATED = ZOO_CREATED_EVENT;
const int zkClient::EventType::DELETED = ZOO_DELETED_EVENT;
const int zkClient::EventType::CHANGED = ZOO_CHANGED_EVENT;
const int zkClient::EventType::CHILD   = ZOO_CHILD_EVENT;
const int zkClient::EventType::SESSION = ZOO_SESSION_EVENT;
const int zkClient::EventType::NOWATCHING = ZOO_NOTWATCHING_EVENT;

const int zkClient::FlagsType::EPHEMERAL = ZOO_EPHEMERAL;
const int zkClient::FlagsType::SEQUENCE  = ZOO_SEQUENCE;
// const int zkClient::FlagsType::CONTAINER = CONTAINER;

const int zkClient::StateType::EXPIRED_SESSION = ZOO_EXPIRED_SESSION_STATE;
const int zkClient::StateType::AUTH_FAILED = ZOO_AUTH_FAILED_STATE;
const int zkClient::StateType::CONNECTING = ZOO_CONNECTING_STATE;
const int zkClient::StateType::ASSOCIATING = ZOO_ASSOCIATING_STATE;
const int zkClient::StateType::CONNECTED = ZOO_CONNECTED_STATE;
// const int zkClient::StateType::READONLY = ZOO_READONLY_STATE;
// const int zkClient::StateType::NOTCONNECTED = ZOO_NOTCONNECTED_STATE;

zkClient::zkClient()
    :m_handle(nullptr)
    ,m_recvTimeout(0) {
}

zkClient::~zkClient() {
    if(m_handle) {
        close();
    }
}

void zkClient::OnWatcher(zhandle_t *zh, int type, int stat, const char *path,void *watcherCtx) {
    zkClient* client = (zkClient*)watcherCtx;
    client->m_watcherCb(type, stat, path);
}

bool zkClient::reconnect() {
    if(m_handle) {
        zookeeper_close(m_handle);
    }
    m_handle = zookeeper_init(m_hosts.c_str(), &zkClient::OnWatcher, m_recvTimeout, nullptr, this, 0);
    // m_handle = zookeeper_init2(m_hosts.c_str(), &zkClient::OnWatcher, m_recvTimeout, nullptr, this, 0, m_logCb);
    return m_handle != nullptr;
}

bool zkClient::init(const std::string& host, int recv_timeout, watcher_callback cb) {
    if(m_handle) {
        return true;
    }
    m_hosts = host;
    m_recvTimeout = recv_timeout;
    if(cb != nullptr) {
        m_watcherCb = std::bind(cb, std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3,
                                shared_from_this());
    }
    m_handle = zookeeper_init(m_hosts.c_str(), &zkClient::OnWatcher, m_recvTimeout, nullptr, this, 0);
    // m_handle = zookeeper_init2(hosts.c_str(), &zkClient::OnWatcher, m_recvTimeout, nullptr, this, 0, lcb);
    return m_handle != nullptr;
}

int32_t zkClient::create(const std::string& path, const std::string& val, std::string& new_path
                         ,const struct ACL_vector* acl
                         ,int flags) {
    return zoo_create(m_handle, path.c_str(), val.c_str(), val.size(), acl, flags, &new_path[0], new_path.size());
}

int32_t zkClient::exists(const std::string& path, bool watch, Stat* stat) {
    return zoo_exists(m_handle, path.c_str(), watch, stat);
}

int32_t zkClient::del(const std::string& path, int version) {
    return zoo_delete(m_handle, path.c_str(), version);
}

int32_t zkClient::get(const std::string& path, std::string& val, bool watch, Stat* stat) {
    int len = val.size();
    int32_t rt = zoo_get(m_handle, path.c_str(), watch, &val[0], &len, stat);
    if(rt == ZOK) {
        val.resize(len);
    }
    return rt;
}

int32_t zkClient::set(const std::string& path, const std::string& val, int version, Stat* stat) {
    return zoo_set2(m_handle, path.c_str(), val.c_str(), val.size(), version, stat);
}

int32_t zkClient::getChildren(const std::string& path, std::vector<std::string>& val, bool watch, Stat* stat) {
    String_vector strings;
    Stat tmp;
    if(stat == nullptr) {
        stat = &tmp;
    }
    int32_t rt = zoo_get_children2(m_handle, path.c_str(), watch, &strings, stat);
    if(rt == ZOK) {
        for(int32_t i = 0; i < strings.count; ++i) {
            val.push_back(strings.data[i]);
        }
        deallocate_String_vector(&strings);
    }
    return rt;
}

int32_t zkClient::close() {
    m_watcherCb = nullptr;
    int32_t rt = ZOK;
    if(m_handle) {
        rt = zookeeper_close(m_handle);
        m_handle = nullptr;
    }
    return rt;
}

int32_t zkClient::getState() {
    return zoo_state(m_handle);
}

}
