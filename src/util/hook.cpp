#include "util/hook.h"

#include <dlfcn.h>
#include "log/logger.h"
#include "fiber/fiber.h"
#include "eventpoller/eventpoller.h"

sylar::Logger::Ptr g_logger = Name_Logger("system");

namespace sylar {

static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
XX(sleep) \
XX(usleep) \
XX(nanosleep) \
XX(socket) \
XX(connect) \
XX(accept) \
XX(read) \
XX(readv) \
XX(recv) \
XX(recvfrom) \
XX(recvmsg) \
XX(write) \
XX(writev) \
XX(send) \
XX(sendto) \
XX(sendmsg) \
XX(close) \
XX(fcntl) \
XX(ioctl) \

void hook_init() {
    static bool is_inited = false;
    if(is_inited) {
        return;
    }
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
}

static uint64_t s_connect_timeout = -1;
struct _HookIniter {
    _HookIniter() {
        hook_init();
    }
};

static _HookIniter s_hook_initer;

bool is_hook_enable() {
    return t_hook_enable;
}

void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}

struct timer_info {
    int cancelled = 0;
};

// ???
template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
        uint32_t event, int timeout_so, Args&&... args) {
    if(!sylar::t_hook_enable) {
        return fun(fd, std::forward<Args>(args)...);
    }

    sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
    if(!ctx) {
        return fun(fd, std::forward<Args>(args)...);
    }

    if(ctx->isClose()) {
        errno = EBADF;
        return -1;
    }

    if(!ctx->isSocket() || ctx->getUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t to = ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);

retry:
    ssize_t n = fun(fd, std::forward<Args>(args)...);
    while(n == -1 && errno == EINTR) {
        n = fun(fd, std::forward<Args>(args)...);
    }
    if(n == -1 && errno == EAGAIN) {
        sylar::IOManager* iom = sylar::IOManager::GetThis();
        sylar::Timer::ptr timer;
        std::weak_ptr<timer_info> winfo(tinfo);

        if(to != (uint64_t)-1) {
            timer = iom->addConditionTimer(to, [winfo, fd, iom, event]() {
                auto t = winfo.lock();
                if(!t || t->cancelled) {
                    return;
                }
                t->cancelled = ETIMEDOUT;
                iom->cancelEvent(fd, (sylar::IOManager::Event)(event));
            }, winfo);
        }

        int rt = iom->addEvent(fd, (sylar::IOManager::Event)(event));
        if(SYLAR_UNLIKELY(rt)) {
            SYLAR_LOG_ERROR(g_logger) << hook_fun_name << " addEvent("
                << fd << ", " << event << ")";
            if(timer) {
                timer->cancel();
            }
            return -1;
        } else {
            sylar::Fiber::YieldToHold();
            if(timer) {
                timer->cancel();
            }
            if(tinfo->cancelled) {
                errno = tinfo->cancelled;
                return -1;
            }
            goto retry;
        }
    }
    
    return n;
}

} // namespace sylar

extern "C" {
    #define XX(name) name ## _fun name ## _f = nullptr;
        HOOK_FUN(XX);
    #undef XX
}

unsigned int sleep(unsigned int secs) {
    if(!sylar::t_hook_enable) {
        return sleep_f(secs);
    }
    sylar::Fiber::Ptr fiber = sylar::Fiber::getThis();
    sylar::EventPoller* ep = sylar::EventPoller::getThis();
    ep->addTimer(secs * 1000, [ep, fiber]() {
        ep->schedule(fiber);
    });
    sylar::Fiber::yieldToHold();
    return 0;
}

int usleep(useconds_t usec) {
    if(!sylar::t_hook_enable) {
        return usleep_f(usec);
    }
    sylar::Fiber::Ptr fiber = sylar::Fiber::getThis();
    sylar::EventPoller* ep = sylar::EventPoller::getThis();
    ep->addTimer(usec / 1000, [ep, fiber]() {
        ep->schedule(fiber);
    });
    sylar::Fiber::yieldToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
    if(!sylar::t_hook_enable) {
        return nanosleep_f(req, rem);
    }
    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
    sylar::Fiber::Ptr fiber = sylar::Fiber::getThis();
    sylar::EventPoller* ep = sylar::EventPoller::getThis();
    ep->addTimer(timeout_ms, [ep, fiber]() {
        ep->schedule(fiber);
    });
    sylar::Fiber::yieldToHold();
    return 0;
}