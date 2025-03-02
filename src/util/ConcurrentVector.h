#ifndef _SYLAR_CONCURRENT_VECTOR_H_
#define _SYLAR_CONCURRENT_VECTOR_H_

#include <memory>
#include <vector>
#include <atomic>
#include <functional>
#include <shared_mutex>

#include "util/macro.h"
#include "util/util.h"

namespace sylar {

template<class ItemType>
class ConcurrentVector : public noncopyable{
public:
    using Ptr = std::shared_ptr<ConcurrentVector>;
    using MutexType = std::shared_mutex;
    using MutexPtr = std::shared_ptr<MutexType>;
    ConcurrentVector(size_t size = 0, size_t granu = 16) {
        Assert((size >= 0 && granu > 0));
        m_size.store(size);
        m_granu.store(granu);
        if(m_size / m_granu > m_mtx_size) {
            m_mtx_size = m_size / m_granu;
        }
        m_vector.reserve(m_size);
        m_mtx.resize(m_mtx_size);
        for(int i = 0; i < m_mtx_size; ++i) {
            m_mtx[i].reset(new MutexType);
        }
    }
    ~ConcurrentVector() {
        std::cout << "~ConcurrentVector()\n";
    };

    ItemType operator[](size_t index) const {
        if(index < 0 || index >= m_size) {
            throw std::out_of_range("ConcurrentVector[" + std::to_string(index) + "] indexout of range");
        }
        size_t mtx_idx = hashFunc(index) % m_mtx_size;
        std::shared_lock<MutexType> lock(*m_mtx[mtx_idx]);
        return m_vector[index];
    }

    void set(size_t index, const ItemType& val) {
        if(index < 0 || index >= m_size) {
            throw std::out_of_range("ConcurrentVector::set("  + std::to_string(index) + "," + std::to_string(val)
                                     + ") index out of range");
        }
        size_t mtx_idx = hashFunc(index) % m_mtx_size;
        std::unique_lock<MutexType> lock(*m_mtx[mtx_idx]);
        m_vector[index] = val;
    }

    void push_back(const ItemType& val) {
        m_size.fetch_add(1);
        check_mtx();
        size_t mtx_idx = hashFunc(m_size.load()) % m_mtx_size.load();
        std::unique_lock<MutexType> lock(*m_mtx[mtx_idx]);
        m_vector.push_back(val);
    }

    void emplace_back(const ItemType& val) {
        m_size.fetch_add(1);
        check_mtx();
        size_t mtx_idx = hashFunc(m_size.load()) % m_mtx_size.load();
        std::unique_lock<MutexType> lock(*m_mtx[mtx_idx]);
        m_vector.emplace_back(val);
    }
    
    size_t size() const {return m_size.load();}
private:
    void check_mtx() {
        std::unique_lock<MutexType> lock(m_check_mtx);
        size_t load = m_mtx_size.load() * m_granu.load();
        if(m_size > load) {
            m_mtx.emplace_back();
            m_mtx_size.fetch_add(1);
        }
    }

private:
    std::hash<int> hashFunc;
    
    std::atomic<size_t> m_size = 0;
    std::atomic<size_t> m_granu = 16;
    std::atomic<size_t> m_mtx_size = 8;

    std::vector<ItemType> m_vector;

    MutexType m_check_mtx;
    std::vector<MutexPtr> m_mtx;
};

} // namespace sylar


#endif //_SYLAR_CONCURRENT_VECTOR_H_