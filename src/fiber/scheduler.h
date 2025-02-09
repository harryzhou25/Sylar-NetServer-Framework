#ifndef _SYLAR_SCHEDULER_H
#define _SYLAR_SCHEDULER_H

#include <list>
#include <mutex>
#include <memory>
#include <vector>
#include <string>
#include <functional>

#include "fiber/fiber.h"
#include "thread/thread.h"

namespace sylar {
    
class Scheduler {
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
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");

    virtual ~Scheduler();

    const std::string& getName() { return std::move(m_name); }

    void start();

    void stop();

    void tickle();

    template<class T>
    void schedule(T cb, int thread = -1);

    template<class Iterator>
    void schedule(Iterator begin, Iterator end);

protected:
    void run();

    template<class T>
    bool scheduleNonLock(T cb, int thread = -1);

    void idle();

    virtual bool stopping();
public:
    static Scheduler* getThis();

    void setThis();

    static Fiber* getMainFiber();    
private:
    bool m_running = false;

    bool m_autoStop = false;

    bool m_stopping = false;

    size_t m_threadNum;

    size_t m_idelThreadNum;

    size_t m_activeThreadNum;

    std::string m_name;

    std::mutex m_mtx;

    int m_rootThread;

    std::vector<int> m_threadIds;

    std::vector<Thread::Ptr> m_threads;

    Fiber::Ptr m_mainFiber; //调度协程

    std::list<FiberAndThread> m_fibers; //重封装 优化取出不属于自己的线程的协程的情况
};

} // namespace sylar


#endif //_SYLAR_SCHEDULER_H