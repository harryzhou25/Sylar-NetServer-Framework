#include "skipList/skipList.hpp"
#include <iostream>
namespace sylar {

template <class K, class V>
SkipList<K, V>::Node::Node(const K k, const V v, size_t level)
    :m_key(k), m_value(v), m_level(level) {
    m_next.resize(m_level+1, nullptr);
}

template <class K, class V>
K SkipList<K, V>::Node::getKey() {
    std::shared_lock<MutexType> lock(m_mtx);
    return m_key;
}

template <class K, class V>
V SkipList<K, V>::Node::getValue() {
    std::shared_lock<MutexType> lock(m_mtx);
    return m_value;
}

template <class K, class V>
size_t SkipList<K, V>::Node::getLevel() {
    std::shared_lock<MutexType> lock(m_mtx);
    return m_level;
}

template <class K, class V>
void SkipList<K, V>::Node::setValue(V val) {
    std::unique_lock<MutexType> lock(m_mtx);
    m_value = val;
}

template <class K, class V>
bool SkipList<K, V>::Node::find(int level) {
    if(m_next.size() > level) {
        return (m_next[level] != nullptr);
    }
    return false;
}

template <class K, class V>
void SkipList<K, V>::Node::resize(int level) {
    m_next.resize(level+1);
    for(int i = m_level+1; i <= level; ++i) {
        m_next[i] = nullptr;
    }
    m_level = level;
}

template <class K, class V>
int SkipList<K, V>::getRandomLevel() {
    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = std::min(k, m_max_level);
    return k;
}

template<class K, class V>
SkipList<K,V>::SkipList(int max_level)
    : m_max_level(max_level), m_length(0), m_current_level(1) {
    K k;
    V v;
    m_head = typename Node::Ptr(new Node(k, v, m_max_level));
}

template<class K, class V>
bool SkipList<K, V>::searchValue(K key, V& val) {
    auto cur = m_head;
    for(int i = m_current_level; i >= 0 ; --i) {
        while(cur->find(i) && cur->m_next[i]->getKey() < key) {
            cur = cur->m_next[i];
        }
    }
    cur = cur->m_next[0];
    if (cur && cur->getKey() == key) {
        val = cur->getValue();
        return true;
    }
    return false;
}

template<class K, class V>
bool SkipList<K, V>::search(K key) {
    V v;
    return searchValue(key, v);
}

template<class K, class V>
void SkipList<K, V>::insert(K key, V val) {
    std::unique_lock<std::shared_mutex> lock(m_listMtx);
    auto cur = m_head;
    std::vector<typename Node::Ptr> update(m_max_level+1, nullptr);
    for(int i = m_current_level; i >= 0 ; --i) {
        while(cur->find(i) && cur->m_next[i] && cur->m_next[i]->getKey() < key) {
            cur = cur->m_next[i];
        }
        update[i] = cur;
    }
    cur = cur->m_next[0];
    if(cur && cur->getKey() == key) {
        cur->setValue(val);   
    }
    else {
        int random_level = getRandomLevel();
        if(random_level > m_current_level) {
            for(int i = m_current_level+1; i <= random_level; ++i) {
                update[i] = m_head;
            }
        }
        typename Node::Ptr new_node(new Node(key, val, random_level));
        m_current_level = std::max(m_current_level, random_level);
        for(int i = 0; i <= random_level; ++i) {
            if(update[i]) {
                new_node->m_next[i] = update[i]->m_next[i];
                update[i]->m_next[i] = new_node;
            }
        }
        ++m_length;
    }
}

template<class K, class V>
void SkipList<K, V>::erase(K key) {
    std::unique_lock<std::shared_mutex> lock(m_listMtx);
    auto cur = m_head;
    std::vector<typename Node::Ptr> update(m_current_level+1, nullptr);
    for(int i = m_current_level; i>=0; --i) {
        while(cur->find(i) && cur->m_next[i]->getKey() < key) {
            cur = cur->m_next[i];
        }
        update[i] = cur;
    }
    cur = cur->m_next[0];
    if(cur && cur->getKey() == key) {
        for(int i = 0; i <= m_current_level; ++i) {
            if(update[i]->m_next[i] != cur) break;
            update[i]->m_next[i] = cur->m_next[i];
        }
    }
    while(m_current_level && m_head->m_next[m_current_level] == nullptr) {
        --m_current_level;
    }
}

template class SkipList<std::string, std::string>;
} // namespace sylar
