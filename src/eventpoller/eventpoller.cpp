#include "eventpoller/eventpoller.h"
#include "log/logger.h"
#include "util/macro.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

namespace sylar {

static Logger::Ptr g_logger = Name_Logger("system");

EventPoller::EventPoller(size_t threads, bool use_caller, const std::string& name) 
    :Scheduler(threads, use_caller, name) {
    m_epfd = epoll_create(5000);
    Assert((m_epfd > 0));

    int rt = pipe(m_tickleFds);
    Assert(!rt);
    
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    Assert(!rt);

    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    Assert(!rt);

    contextResize(32);
    
    start();
}

EventPoller::~EventPoller() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for(size_t i = 0; i < m_fdContexts.size(); ++i) {
        if(m_fdContexts[i]) {
            delete m_fdContexts[i];
        }
    }
}

EventPoller* EventPoller::getThis() {
    return dynamic_cast<EventPoller*>(Scheduler::getThis());
}

bool EventPoller::stopping() {
    return m_pendingEventCount == 0
        && !hasTimer()
        && Scheduler::stopping();

}

void EventPoller::contextResize(size_t size) {
    // writeLock(m_mtx);
    std::unique_lock<MutexType> lock(m_mtx);
    if(size <= m_fdContexts.size())  {
        return;
    }
    m_fdContexts.resize(size);
    for(int i = 0; i < size; ++i) {
        if(!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

void EventPoller::FdContext::triggerEvent(Event event) {
    Assert((events & event));

    events = (Event)(events & ~event);
    EventContext& ctx = getContext(event);
    if(ctx.cb) {
        ctx.scheduler->schedule(&ctx.cb);
    } 
    else {
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
}

EventPoller::FdContext::EventContext& EventPoller::FdContext::getContext(EventPoller::Event event) {
    switch(event) {
        case EventPoller::READ:
            return read;
        case EventPoller::WRITE:
            return write;
        default:
            Assert_Commit(false, "getContext");
    }
    throw std::invalid_argument("getContext invalid event");
}

void EventPoller::FdContext::resetContext(EventContext& ctx) {
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

int EventPoller::addEvent(int fd, Event event, std::function<void()> cb) {
    FdContext* fd_ctx = nullptr;
    std::shared_lock<MutexType> readlock(m_mtx);
    if((int)m_fdContexts.size() > fd) {
        fd_ctx = m_fdContexts[fd];
        readlock.unlock();
    } else {
        readlock.unlock();
        std::unique_lock<MutexType> writelock(m_mtx);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }

    std::lock_guard<FdContext::MutexType> fd_lock(fd_ctx->mutex);
    if(fd_ctx->events & event) {
        Log_Error(g_logger) << "addEvent assert fd=" << fd
                    << " event=" << (EPOLL_EVENTS)event
                    << " fd_ctx.event=" << (EPOLL_EVENTS)fd_ctx->events;
        Assert(!(fd_ctx->events & event));
    }

    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events | event;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt) {
        Log_Error(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ") fd_ctx->events="
            << (EPOLL_EVENTS)fd_ctx->events;
        return -1;
    }

    ++m_pendingEventCount;
    fd_ctx->events = (Event)(fd_ctx->events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    Assert((!event_ctx.scheduler
                && !event_ctx.fiber
                && !event_ctx.cb));

    event_ctx.scheduler = Scheduler::getThis();
    if(cb) {
        event_ctx.cb.swap(cb);
    } else {
        event_ctx.fiber = Fiber::getThis();
        Assert((event_ctx.fiber->getState() == Fiber::EXEC));
    }
    return 0;
}

bool EventPoller::delEvent(int fd, Event event) {
    //readLock(m_mtx);
    std::shared_lock<MutexType> lock(m_mtx);
    if((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    m_mtx.unlock_shared();
    
    std::lock_guard<std::mutex> lokc(fd_ctx->mutex);
    if(!(fd_ctx->events & event)) {
        return false;
    }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;
    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt) {
        Log_Error(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    --m_pendingEventCount;
    fd_ctx->events = new_events;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

bool EventPoller::cancelEvent(int fd, Event event) {
    // readLock(m_mtx);
    std::shared_lock<MutexType> lock(m_mtx);
    if((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    // readUnLock(m_mtx);
    m_mtx.unlock_shared();
    
    std::lock_guard<std::mutex> lokc(fd_ctx->mutex);
    if(!(fd_ctx->events & event)) {
        return false;
    }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt) {
        Log_Error(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    --m_pendingEventCount;
    fd_ctx->triggerEvent(event);
    return true;
}

bool EventPoller::cancelAll(int fd) {
    //readLock(m_mtx);
    std::shared_lock<MutexType> lock(m_mtx);
    if((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    // readUnLock(m_mtx);
    m_mtx.unlock_shared();
    
    std::lock_guard<std::mutex> lokc(fd_ctx->mutex);
    if(!(fd_ctx->events)) {
        return false;
    }

    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if(rt) {
        Log_Error(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
            << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    if(fd_ctx->events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    if(fd_ctx->events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }

    Assert((fd_ctx->events == 0));
    return true;
}

void EventPoller::tickle() {
    int rt = write(m_tickleFds[1], "T", 1);
    Assert((rt == 1));
}

void EventPoller::idle() {
    const uint64_t MAX_EVNETS = 256;
    epoll_event* events = new epoll_event[MAX_EVNETS]();
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
        delete[] ptr;
    });

    int rt = 0;
    while(true) {
        if(stopping()) {
            if(!hasTimer()) {
                break;
            }
        }
        do {
            uint64_t next_timeout = getNextTimer();
            if(next_timeout == 0) {
                next_timeout = MAX_TIMEOUT;
            }
            else {
                if(next_timeout > MAX_TIMEOUT) {
                    next_timeout = MAX_TIMEOUT;
                }
            }
            rt = epoll_wait(m_epfd, events, 64, int(next_timeout));
            if(rt < 0 && errno == EINTR) {
                continue;
            }
            else {
                break;
            }
        } while(true);

        std::vector<Func> cbs;
        listExpiredCb(cbs);
        if(!cbs.empty()) {
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for(int i = 0; i < rt; ++i) {
            epoll_event& event = events[i];
            if(event.data.fd == m_tickleFds[0]) {
                uint8_t dummy[256];
                while(read(m_tickleFds[0], dummy, sizeof(dummy)) > 0);
                continue;
            }

            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            std::lock_guard<FdContext::MutexType> lock(fd_ctx->mutex);
            if(event.events & (EPOLLERR | EPOLLHUP)) {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
            }
            int real_events = NONE;
            if(event.events & EPOLLIN) {
                real_events |= READ;
            }
            if(event.events & EPOLLOUT) {
                real_events |= WRITE;
            }

            if((fd_ctx->events & real_events) == NONE) {
                continue;
            }
            
            int left_events = (fd_ctx->events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;
            
            int _rt = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if(_rt) {
                Log_Debug(g_logger) << "epoll_ctl(" << m_epfd << ", "
                    << op << ", " << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events << "):"
                    << _rt << " (" << errno << ") (" << strerror(errno) << ")";
                continue;
            }
            if(real_events & READ) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if(real_events & WRITE) {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }

        Fiber::Ptr cur = Fiber::getThis();
        auto raw_ptr = cur.get();
        cur.reset();

        raw_ptr->swapOut();
    }
}

void EventPoller::flushTimer() {
    tickle();
}

} // namespace sylar
