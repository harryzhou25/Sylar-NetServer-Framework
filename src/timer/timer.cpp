#include "timer/timer.h"
#include "util/util.h"
#include "log/logger.h"

namespace sylar {
static Logger::Ptr g_logger = Name_Logger("system");

bool Timer::Comparator::operator()(const Timer::Ptr& lhs, const Timer::Ptr& rhs) const{
    if(!lhs && !rhs) {
        return false;
    }
    if(!lhs) {
        return false;
    }
    if(!rhs) {
        return true;
    }
    if(lhs->m_next != rhs->m_next) {
        return (lhs->m_next < rhs->m_next);
    }
    else {
        return (lhs.get() < rhs.get());
    }
}

Timer::Timer(uint64_t ms, std::function<void()> cb, 
            bool recurring, TimerManager* manager)
            : m_ms(ms), m_recurring(recurring), m_manager(manager) {
    m_cb = std::forward<std::function<void()>>(cb);
    m_next = getCurrentMS() + ms;
}

Timer::Timer(uint64_t next)
    :m_next(next) {
}

bool Timer::cancel() {
    std::unique_lock<TimerManager::Mutex> lock(m_manager->m_mtx);
    if(m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        if(it != m_manager->m_timers.end()) {
            m_manager->m_timers.erase(it);
        }
        return true;
    }
    return false;
}

bool Timer::refresh() {
    {
        std::shared_lock<TimerManager::Mutex> lock(m_manager->m_mtx);
        if(!m_cb) {
            return false;
        }
    }
    {
        std::unique_lock<TimerManager::Mutex> lock(m_manager->m_mtx);
        auto it = m_manager->m_timers.find(shared_from_this());
        if(it != m_manager->m_timers.end()) {
            return false;
        }
        m_manager->m_timers.erase(it);
        m_next = sylar::getCurrentMS() + m_ms;
        m_manager->m_timers.insert(shared_from_this());
        return true;
    }
}

bool Timer::reset(uint64_t ms, bool from_now) {
    if(ms == m_ms && !from_now) {
        return true;
    }
    std::unique_lock<TimerManager::Mutex> lock(m_manager->m_mtx);
    if(!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if(it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if(from_now) {
        start = sylar::getCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    
    return true;
}

Timer::Ptr TimerManager::addTimer(uint64_t ms, Func cb, bool recurring) {
    Timer::Ptr timer(new Timer(ms, cb, recurring, this));
    bool at_front;
    {
        std::shared_lock<Mutex> lock(m_mtx);
        auto it = m_timers.insert(timer).first;
        at_front = (it == m_timers.begin());
    }
    if(at_front) {
        flushTimer();
    }
    return timer;
}

void TimerManager::addTimer(Timer::Ptr val, Mutex& lock) {
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin());
    lock.unlock();
    if(at_front) {
        flushTimer();
    }
}

static void TimerCondCheck(std::weak_ptr<void> cond, std::function<void()> cb) {
    auto tmp = cond.lock();
    if(tmp) {
        cb();
    }
}

Timer::Ptr TimerManager::addConditionTimer(uint64_t ms, Func cb, std::weak_ptr<void> cond, bool recurring) {
    return addTimer(ms, [cond, cb]() {
        TimerCondCheck(cond, cb);
    }, recurring);
}

uint64_t TimerManager::getNextTimer() {
    std::shared_lock<Mutex> lock(m_mtx);
    m_tickled = false;
    if(m_timers.empty()) {
        return 0;
    }
    const auto nxt = *m_timers.begin();
    auto cur_ms = getCurrentMS();
    if(nxt->m_next >= cur_ms) {
        return nxt->m_next - cur_ms;
    }
    else {
        return 0;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
    auto cur_ms = getCurrentMS();
    std::vector<Timer::Ptr> expired;
    {
        std::shared_lock<Mutex> lock(m_mtx);
        if(m_timers.empty()) {
            return;
        }
    }
    std::unique_lock<Mutex> lock(m_mtx);
    Timer::Ptr cur_timer(new Timer(cur_ms));
    auto it = m_timers.lower_bound(cur_timer);
    while(it != m_timers.end() && (*it)->m_next == cur_ms) {
        ++it;
    }
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());
    for(auto & timer : expired) {
        cbs.emplace_back(timer->m_cb);
        if(timer->m_recurring) {
            timer->m_next = cur_ms + timer->m_ms;
            m_timers.insert(timer);
        }
        else {
            timer->m_cb = nullptr;
        }
    }
}

bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if(now_ms < m_prevTime &&
            now_ms < (m_prevTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_prevTime = now_ms;
    return rollover;
}

bool TimerManager::hasTimer() {
    std::shared_lock<Mutex> lock(m_mtx);
    return (m_timers.empty() == false);
}

} // namespace sylar
