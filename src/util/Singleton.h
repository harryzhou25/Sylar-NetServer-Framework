#ifndef _SYLAR_SINGLETON_H
#define _SYLAR_SINGLETON_H

#include <mutex>
#include <memory>
#include <iostream>

namespace sylar {
    
template<class T>
class Singleton {
private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
public:
    static T* getInstance() {
        std::lock_guard<std::mutex> lock(m_mtx);
        if(m_instance == nullptr) {
            m_instance = new T();
        }
        return m_instance;
    }
private:
    static T* m_instance;
    static std::mutex m_mtx;
};

template<class T>
T* Singleton<T>::m_instance = nullptr;

template<class T>
std::mutex Singleton<T>::m_mtx;

template<class T>
class SingletonPtr {
private:
    SingletonPtr() = default;
    SingletonPtr(const SingletonPtr&) = delete;
    SingletonPtr& operator=(const SingletonPtr&) = delete;
public:
    static std::shared_ptr<T> getInstance() {
        std::lock_guard<std::mutex> lock(m_mtx);
        if(!m_instance) {
            m_instance = std::make_shared<T>();
        }
        return m_instance;
    }
private:
    static std::shared_ptr<T> m_instance;
    static std::mutex m_mtx;
};

template<class T>
std::shared_ptr<T> SingletonPtr<T>::m_instance = nullptr;

template<class T>
std::mutex SingletonPtr<T>::m_mtx;

} // namespace sylar


#endif //_SYLAR_SINGLETON_H