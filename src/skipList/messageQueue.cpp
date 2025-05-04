#include "skipList/messageQueue.h"

namespace sylar {

template<class K, class V>
messageQueue<K, V>::messageQueue() {
    m_map = std::shared_ptr<MapType>(new MapType(5));
}

template<class K, class V>
void messageQueue<K, V>::insert(K key) {
    std::deque<V> q;
    m_map->insert(key, q);
}

template<class K, class V>
size_t messageQueue<K, V>::size(K key) {
    std::deque<V> q;
    m_map->searchValue(key, q);
    return q.size();
}

template<class K, class V>
V messageQueue<K, V>::back(K key) {
    std::deque<V> q;
    m_map->searchValue(key, q);
    return q.back();
}

template<class K, class V>
V messageQueue<K, V>::front(K key) {
    std::deque<V> q;
    m_map->searchValue(key, q);
    return q.front();
}

template<class K, class V>
void messageQueue<K, V>::push_back(K key, V val) {
    std::deque<V> q;
    m_map->searchValue(key, q);
    q.push_back(val);
}

template<class K, class V>
void messageQueue<K, V>::push_front(K key, V val) {
    std::deque<V> q;
    m_map->searchValue(key, q);
    q.push_front(val);
}

template<class K, class V>
void messageQueue<K, V>::pop_back(K key) {
    std::deque<V> q;
    m_map->searchValue(key, q);
    q.pop_back();
}

template<class K, class V>
void messageQueue<K, V>::pop_front(K key) {
    std::deque<V> q;
    m_map->searchValue(key, q);
    q.pop_front();
}

} // namespace sylar
