#ifndef _SYLAR_SKIPLIST_H_
#define _SYLAR_SKIPLIST_H_

#include <memory>
#include <vector>
#include <mutex>
#include <algorithm>
#include <shared_mutex>

namespace sylar {

template <class K, class V> 
class SkipList {
public:
    using Ptr = std::shared_ptr<SkipList<K, V>>;
    SkipList(int max_level);
    ~SkipList() = default;
    int getRandomLevel();
    bool search(K key);
    bool searchValue(K key, V& val);
    void erase(K val);
    void insert(K key, V val);
    int getSize() const {return m_length;};
protected:
    class Node {
        public:
            using Ptr = std::shared_ptr<Node>;
            using MutexType = std::shared_mutex;
            Node() = default;
            ~Node() = default;
            Node(const K k, const V v, size_t level);
            bool find(int level);
            void resize(int level);
            K getKey();
            V getValue();
            size_t getLevel();
            void setValue(V val);
            std::vector<Node::Ptr> m_next;
        private:
            K m_key;
            V m_value;
            size_t m_level;
            MutexType m_mtx;
    };
private:
    int m_max_level;
    int m_length;
    int m_current_level;
    std::shared_mutex m_listMtx;
    typename Node::Ptr m_head;
};
} // namespace sylar


#endif //_SYLAR_SKIPLIST_H_