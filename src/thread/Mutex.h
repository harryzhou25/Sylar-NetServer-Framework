#ifndef _SYLAR_MUTEX_H
#define _SYLAR_MUTEX_H
#include <stdint.h>
#include <semaphore.h>

#include "util/util.h"

namespace sylar {

class semaphore : noncopyable {
public:
    /**
     * @brief 构造函数
     * @param[in] count 信号量值的大小
     */
    semaphore(uint32_t count = 0) {
        if(sem_init(&m_sem, 0, count)) {
            throw std::logic_error("sem_init error");
        }
    }

    /**
     * @brief 析构函数
     */
    ~semaphore() {
        sem_destroy(&m_sem);
    }

    /**
     * @brief 获取信号量
     */
    void wait() {
        sem_wait(&m_sem);
    }

    /**
     * @brief 释放信号量
     */
    void notify(size_t n = 1) {
        while (n--) {
            sem_post(&m_sem);
        }
    }
private:
    sem_t m_sem;
};

} // namespace sylar


#endif //_SYLAR_MUTEX_H