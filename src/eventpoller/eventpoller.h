#ifndef _SYLAR_IOMANAGER_H_
#define _SYLAR_IOMANAGER_H_
#include "fiber/scheduler.h"
#include "thread/Mutex.h"
#include "timer/timer.h"

#include <memory>

namespace sylar {

class EventPoller : public Scheduler, public TimerManager {
public:
    using Ptr = std::shared_ptr<EventPoller>;
    using MutexType = std::shared_mutex;
    enum Event {
        NONE    = 0x0,
        READ    = 0x1,
        WRITE   = 0x4,
    };

private:
    struct FdContext {
        using MutexType = std::mutex;

        struct EventContext {
            Scheduler* scheduler = nullptr;
            Fiber::Ptr fiber;
            std::function<void()> cb;
        };

        EventContext& getContext(Event event);

        void resetContext(EventContext& ctx);

        void triggerEvent(Event event);

        EventContext read;
        EventContext write;
        int fd = 0;
        Event events = NONE;
        MutexType mutex;
    };

public:
    EventPoller(size_t threads = 1, bool use_caller = true, const std::string& name = "");

    ~EventPoller();

    static EventPoller* getThis();

    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

    bool delEvent(int fd, Event event);

    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);

    void tickle() override;

    void idle() override;

    bool stopping() override;

private:
    void contextResize(size_t size);

    void flushTimer() override;

private:
    int m_epfd;
    
    int m_tickleFds[2]; // 读写管道

    MutexType m_mtx;

    std::atomic<size_t> m_pendingEventCount = {0};

    std::vector<FdContext*> m_fdContexts;

    static const int MAX_TIMEOUT = 3000;
};

} // namespace sylar


#endif //_SYLAR_IOMANAGER_H_