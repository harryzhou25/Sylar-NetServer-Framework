#ifndef _SYLAR_SKIPLIST_H_
#define _SYLAR_SKIPLIST_H_

#include<memory>
#include<vector>
#include<mutex>
#include<stdint.h>
#include<algorithm>
#include<shared_mutex>
#include <fstream>
#include <iostream>

namespace sylar {

template <class K, class V> 
class SkipList {
public:
    using Ptr = std::shared_ptr<SkipList<K, V>>;
    using ValPtr = std::shared_ptr<V>;

    SkipList(int max_level)
        : m_max_level(max_level), m_length(0), m_current_level(1), m_curVersion(0) {
        K k;
        V v;
        m_head = typename Node::Ptr(new Node(k, v, m_max_level));
    }

    virtual ~SkipList() = default;
    
    int getRandomLevel() {
        int k = 1;
        while (rand() % 2) {
            k++;
        }
        k = std::min(k, m_max_level);
        return k;
    }

    bool searchValue(K key, V& val) {
        auto cur = m_head;
        for(int i = m_current_level; i >= 0 ; --i) {
            while(cur->find(i) && cur->m_next[i]->getKey() < key) {
                cur = cur->m_next[i];
            }
        }
        cur = cur->m_next[0];
        if (cur && cur->getKey() == key) {
            val = cur->getValuePtr();
            std::cout << key << ' ' << val.size() << '\n';
            return true;
        }
        return false;
    }

    bool search(K key) {
        V v;
        return searchValue(key, v);
    }

    void erase(K key) {
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

    void insert(K key, V&& val) {
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

    void insert(K key, V& val) {
        insert(key, std::move(val));
    }

    void dump() {
        auto cur = m_head->m_next[0];
        std::string cur_addr = m_dumpAddr + std::to_string(m_curVersion);
        std::cout << "dump into " << cur_addr << '\n';
        m_fileWriter.open(cur_addr);
        while(cur) {
            m_fileWriter << '{' << cur->getKey() << ':' << cur->getValue() << ':' << cur->getLevel() << '}' << '\n';
            cur = cur->m_next[0];
        }
        m_fileWriter.close();
        ++m_curVersion;
    }

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

            Node(const K k, const V v, size_t level)
                :m_key(k), m_value(v), m_level(level) {
                m_next.resize(m_level+1, nullptr);
                m_mtxs.resize(m_level+1);
                for(auto &mtx : m_mtxs) {
                    mtx = std::make_shared<MutexType>();
                }
            }

            bool find(int level) {
                if(m_next.size() > level) {
                    return (m_next[level] != nullptr);
                }
                return false;
            }
            void resize(int level) {
                m_next.resize(level+1);
                m_mtxs.resize(level+1);
                for(int i = m_level+1; i <= level; ++i) {
                    m_next[i] = nullptr;
                    m_mtxs[i] = std::make_shared<MutexType>();
                }
                m_level = level;
            }

            K getKey() {
                std::shared_lock<MutexType> lock(*m_mtxs[0]);
                return m_key;
            }

            V getValue() {
                std::shared_lock<MutexType> lock(*m_mtxs[0]);
                return m_value;
            }

            V& getValuePtr() {
                std::shared_lock<MutexType> lock(*m_mtxs[0]);
                return m_value;
            }

            size_t getLevel() {return m_level;}
            
            void setValue(V& val) {setValue(std::move(val));}
            void setValue(V&& val) {
                std::unique_lock<MutexType> lock(*m_mtxs[0]);
                m_value = std::move(val);
            }
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