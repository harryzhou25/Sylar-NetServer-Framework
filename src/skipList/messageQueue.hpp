#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
#include "skipList/skipList.hpp"

#include <memory>
#include <string>
#include <queue>

namespace sylar {

template <class V> 
class messageQueue{
public:
    using Ptr = std::shared_ptr<messageQueue<V>>;
public:
    messageQueue() {m_name = "Message Queue";}
    messageQueue(std::string name): m_name(name){}
    ~messageQueue() = default;

    size_t size() {
        std::shared_lock lock(m_mtx);
        return m_queue.size();
    }

    V back() {
        std::shared_lock lock(m_mtx);
        return m_queue.back();
    }

    V front() {
        std::shared_lock lock(m_mtx);
        return m_queue.front();        
    }

    V pop_back() {
        std::unique_lock lock(m_mtx);
        auto val = m_queue.back();
        m_queue.pop_back();
        return val;
    }

    V pop_front() {
        std::unique_lock lock(m_mtx);
        auto val = m_queue.front();
        m_queue.pop_front();
        return val; 
    }

    void push_back(V val) {
        std::unique_lock lock(m_mtx);
        m_queue.push_back(val);
    }
    void push_front(V val) {
        std::unique_lock lock(m_mtx);
        m_queue.push_front(val);
    }

    void setName(std::string name) {m_name = std::move(name);}
    std::string getName() {return m_name;}
private:
    std::shared_mutex m_mtx;
    std::deque<V> m_queue;
    std::string m_name;
};
} // namespace sylar

#endif //_MESSAGE_QUEUE_H_