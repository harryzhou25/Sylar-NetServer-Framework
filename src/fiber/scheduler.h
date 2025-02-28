#ifndef _SYLAR_SCHEDULER_H
#define _SYLAR_SCHEDULER_H

#include <list>
#include <mutex>
#include <memory>
#include <vector>
#include <string>
#include <functional>

#include "util/hook.h"
#include "fiber/fiber.h"
#include "thread/thread.h"

namespace sylar {

class LoadCounter {
public:
    using MutexType = std::shared_mutex;
    LoadCounter(size_t max_size = 10);

    ~LoadCounter() = default;

    int getLoad();
    
protected:
    void startSleep();

    void startWork();

    private:
    struct TimeRecord {
        TimeRecord(uint64_t tm, bool slp) {
            sleep = slp;
            duration = tm;
        }

        bool sleep;
        uint64_t duration;
    };
private:
    bool m_sleeping = true;
    
    size_t m_max_size;
    
    uint64_t m_last_wake;
    uint64_t m_last_sleep;

    MutexType m_mtx;
    std::list<TimeRecord> m_records;
};

class Scheduler : public LoadCounter {
public:
    using Ptr = std::shared_ptr<Scheduler>;
    using Mutextype = std::mutex;
    using FuncType = std::function<void()>;
public:
    struct FiberAndThread {
        Fiber::Ptr fiber;
        FuncType cb;
        int thread_id;

        FiberAndThread(): thread_id(-1) {}

        FiberAndThread(Fiber::Ptr _fiber, int _thread)
            : fiber(_fiber), thread_id (_thread) {}

        FiberAndThread(Fiber::Ptr* _fiber, int _thread) : thread_id(_thread) {
            fiber.swap(*_fiber);
        }

        FiberAndThread(std::function<void()> _cb, int _thread)  : thread_id(_thread) {
            cb = std::forward<FuncType>(_cb);
        }

        FiberAndThread(std::function<void()>* _cb, int _thread) : thread_id(_thread) {
            cb.swap(*_cb);
        }

        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread_id = -1;
        }
    };

public:
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "test", const size_t load_size = 10);

    virtual ~Scheduler();

    const std::string& getName() { return std::move(m_name); }

    void start();

    void stop();

    template<class T>
    void schedule(T cb, int thread = -1) {
        bool need_tickle = false;
        {
            std::unique_lock<std::shared_mutex> lock(m_threads_mtx);
            need_tickle = need_tickle | scheduleNonLock(cb, thread);
        }
        if(need_tickle) {
            tickle();
        }
    }

    template<class Iterator>
    void schedule(Iterator begin, Iterator end) {
        bool need_tickle = false;
        {
            std::unique_lock<std::shared_mutex> lock(m_threads_mtx);
            while(begin != end) {
                need_tickle = need_tickle | scheduleNonLock(&(*begin), -1);
                ++begin;
            }
        }
        if(need_tickle) {
            tickle();
        }
    }

protected:
    template<class T>
    bool scheduleNonLock(T cb, int thread = -1) {
        bool need_tickle = m_fibers.empty();
        auto item = FiberAndThread(cb, thread);
        if(item.cb || item.fiber) {
            m_fibers.emplace_back(item);
        }
        return need_tickle;
    }

    void run();

    virtual void tickle();

    virtual void idle();

    virtual bool stopping();

public:
    static Scheduler* getThis();

    void setThis();

    static Fiber* getMainFiber();    
private:
    bool m_running = false;
    bool m_autoStop = false;
    bool m_stopping = false;

    size_t m_threadNum = 0;
    size_t m_idelThreadNum = 0;
    size_t m_activeThreadNum = 0;

    std::string m_name;

    int m_rootThread;
    Fiber::Ptr m_mainFiber; //调度协程

    std::shared_mutex m_ids_mtx;
    std::vector<int> m_threadIds;

    std::shared_mutex m_threads_mtx;
    std::vector<Thread::Ptr> m_threads;

    std::mutex m_fibers_mtx;
    std::list<FiberAndThread> m_fibers;
};

} // namespace sylar


#endif //_SYLAR_SCHEDULER_H