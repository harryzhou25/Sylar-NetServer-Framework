#ifndef _SYLAR_IOMANAGER_H_
#define _SYLAR_IOMANAGER_H_
#include "fiber/scheduler.h"
#include "thread/mutex.h"

#include <memory>

namespace sylar {

class IOmanager : public Scheduler {
public:
    using Ptr = std::shared_ptr<IOmanager>;
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
    IOmanager(size_t threads = 1, bool use_caller = true, const std::string& name = "");

    ~IOmanager();

    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

    bool delEvent(int fd, Event event);

    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);

private:
    void contextResize(size_t size);

private:
    int m_epfd;
    
    int m_tickleFds[2];

    MutexType m_mtx;

    std::atomic<size_t> m_pendingEventCount = {0};

    std::vector<FdContext*> m_fdContexts;
};

} // namespace sylar


#endif //_SYLAR_IOMANAGER_H_