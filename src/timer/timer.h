#ifndef _SYLAR_TIMER_H_
#define _SYLAR_TIMER_H_

#include <set>
#include <memory>
#include <shared_mutex>
#include <mutex>
#include <functional>

namespace sylar {

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer>{
friend TimerManager;
public:
    using Ptr = std::shared_ptr<Timer>;
    using Func = std::function<void()>;

private:
    Timer(uint64_t ms, Func cb, bool recurring, TimerManager* manager);
    Timer(uint64_t ms);

public:
    bool cancel();

    bool refresh();

    bool reset(uint64_t ms, bool from_now);

private:
    bool m_recurring = false;
    uint64_t m_ms = 0;
    uint64_t m_next = 0;
    Func m_cb = nullptr;
    TimerManager* m_manager = nullptr;

private:
    struct Comparator {
        bool operator() (const Timer::Ptr& lhs, const Timer::Ptr& rhs) const;
    };
};

class TimerManager {
friend class Timer;
public:
    using Ptr = std::shared_ptr<TimerManager>;
    using Mutex = std::shared_mutex;
    using Func = std::function<void()>;
    
    TimerManager() = default;
    
    virtual ~TimerManager() = default;
    
    Timer::Ptr addTimer(uint64_t ms, Func cb, bool recurring = false);
    
    void addTimer(Timer::Ptr val, Mutex& lock);

    Timer::Ptr addConditionTimer(uint64_t ms, Func cb, std::weak_ptr<void> cond, bool recurring = false);

    uint64_t getNextTimer();

    void listExpiredCb(std::vector<std::function<void()> >& cbs);

    bool detectClockRollover(uint64_t now_ms);

    bool hasTimer();
protected:
    virtual void flushTimer() = 0;

private:
    Mutex m_mtx;
    bool m_tickled = false;
    uint64_t m_prevTime = 0;
    std::set<Timer::Ptr, Timer::Comparator> m_timers;
};

} // namespace sylar


#endif