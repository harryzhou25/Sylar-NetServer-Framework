#ifndef _SYLAR_SKIPLIST_H_
#define _SYLAR_SKIPLIST_H_

#include<memory>
#include<vector>
#include<mutex>
#include<stdint.h>
#include<algorithm>
#include<shared_mutex>
#include <fstream>

namespace sylar {

template <class K, class V> 
class SkipList {
public:
    using Ptr = std::shared_ptr<SkipList<K, V>>;
    using ValPtr = std::shared_ptr<V>;
    SkipList(int max_level);
    virtual ~SkipList() = default;
    int getRandomLevel();
    bool search(K key);
    bool searchValue(K key, V& val_ptr);
    void erase(K val);
    void insert(K key, V& val);
    void insert(K key, V&& val);
    void dump();
    void load();
    void modifyDumpAddr(std::string& addr) {m_dumpAddr = addr;}
    int getSize() const {return m_length;}
protected:
    class Node {
        public:
            using Ptr = std::shared_ptr<Node>;
            using MutexType = std::shared_mutex;
            using MutexPtr = std::shared_ptr<MutexType>;
            Node() = default;
            ~Node() = default;
            Node(const K k, const V v, size_t level);
            bool find(int level);
            void resize(int level);
            K getKey();
            V getValue();
            size_t getLevel();
            void setValue(V& val);
            void setValue(V&& val);
            std::vector<Node::Ptr> m_next;
            std::vector<MutexPtr> m_mtxs;
        private:
            K m_key;
            V m_value;
            MutexType m_mtx;
            size_t m_level;
    };
private:
    int m_length;
    int m_max_level;
    int m_current_level;
    uint64_t m_curVersion;
    std::shared_mutex m_listMtx;
    std::string m_dumpAddr = "../storage/";
    typename Node::Ptr m_head;
    std::ofstream m_fileWriter;
};
} // namespace sylar

#endif //_SYLAR_SKIPLIST_H_