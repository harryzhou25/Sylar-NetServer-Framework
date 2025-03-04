#include "fiber/scheduler.h"
#include "util/macro.h"
#include "util/util.h"
#include "thread/Mutex.h"

namespace sylar {

static Logger::Ptr g_logger = Name_Logger("system");

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler* Scheduler::getThis() {
    return t_scheduler;
}

Fiber* Scheduler::getMainFiber() {
    return t_scheduler_fiber;
}

void Scheduler::setThis() {
    t_scheduler = this;
}

LoadCounter::LoadCounter(size_t max_size) 
    :m_max_size(max_size) {
    m_last_sleep = m_last_wake = getTimeUsec();
}

void LoadCounter::startSleep() {
    m_sleeping = true;
    auto current_time = getTimeUsec();
    auto run_time = current_time - m_last_wake;
    m_last_sleep = current_time;
    {
        std::unique_lock<MutexType> lock(m_mtx);
        m_records.emplace_back(run_time, false);
    }
    if (m_records.size() > m_max_size) {
        m_records.pop_front();
    }
}

void LoadCounter::startWork() {
    m_sleeping = false;
    auto current_time = getTimeUsec();
    auto sleep_time = current_time - m_last_sleep;
    m_last_wake = current_time;
    {
        std::unique_lock<MutexType> lock(m_mtx);
        m_records.emplace_back(sleep_time, true);
    }
    if (m_records.size() > m_max_size) {
        m_records.pop_front();
    }
}

int LoadCounter::getLoad() {
    uint64_t totalSleepTime = 0;
    uint64_t totalRunTime = 0;

    if(m_records.size() == 0) {
        return 0;
    }

    {
        std::shared_lock<MutexType> lock(m_mtx);
        for(auto& i : m_records) {
            if(i.sleep) {
                totalSleepTime += i.duration;
            }
            else {
                totalRunTime += i.duration;
            }
        }
    }

    if(m_sleeping) {
        totalSleepTime += (getTimeUsec() - m_last_sleep);
    } 
    else {
        totalRunTime += (getTimeUsec() - m_last_wake);
    }

    auto totalTime = totalSleepTime + totalRunTime;
    if(totalTime == 0) {
        return 0;
    }
    return int(100 * totalRunTime / totalTime);
}

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name, const size_t load_size) 
                    :m_name(name), LoadCounter(load_size) {
    Assert((threads > 0));
    
    if(use_caller) {
        Fiber::getThis(); // 防止当前线程还没有主协程
        --threads;
        t_scheduler = this;
        
        m_mainFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        t_scheduler_fiber = m_mainFiber.get();
        Log_Debug(g_logger) << "scheduler_fiber id: " << t_scheduler_fiber->getId();

        Thread::t_setName(m_name);
        m_rootThread = getThreadId();
        m_threadIds.emplace_back(m_rootThread);
        Log_Debug(g_logger) << "root thread id " << m_rootThread;
    }
    else {
        m_rootThread = -1;
    }
    m_threadNum = threads;
    Log_Debug(g_logger) << getThreadId() << " created scheduler: " << m_name;
}

Scheduler::~Scheduler() {
    Assert((m_running == false));

    if(!m_autoStop) {
        stop();
    }

    if(Scheduler::getThis() == this) {
        t_scheduler = nullptr;
    }
    Log_Debug(g_logger) << "~Scheduler::Scheduler " << m_name;
}

void Scheduler::start() {
    if(m_running) {
        return;
    }
    Log_Info(g_logger) << "Scheduler::start";
    m_running = true;
    {
        std::unique_lock<std::shared_mutex> lock(m_threads_mtx);
        m_threads.resize(m_threadNum);
        for(size_t i = 0; i < m_threadNum; ++i) {
            m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));
            m_threadIds.emplace_back(m_threads[i]->getId());
        }
    }
}

void Scheduler::stop() {
    Log_Info(g_logger) << "scheduler stop " << getFiberId();
    m_autoStop = true;
    if(m_mainFiber 
        && (m_mainFiber->getState() == Fiber::TERM 
            || m_mainFiber->getState() == Fiber::INIT)
        && m_threadNum == 0) {
        Log_Info(g_logger) << this << " stopped";
        m_running = false;

        if(stopping()) {
            return;
        }
    }

    // 防止线程自己join自己，统一交给根线程处理
    if(m_rootThread != -1) {
        Assert((Scheduler::getThis() == this));
    }
    else {
        Assert((Scheduler::getThis() != this));
    }

    m_running = false;
    std::vector<Thread::Ptr> thrs;
    {
        std::unique_lock<std::shared_mutex> lock(m_threads_mtx);
        thrs.swap(m_threads);
    }

    /* REDO */
    for(auto &i : thrs) {
        i->join();
    }

    if(m_mainFiber) {
        tickle();
    }

    if(!stopping()) {
        m_mainFiber->call();
    }
}

void Scheduler::run() {
    Log_Debug(g_logger) << "Scheduler::run()";
    // startWork();
    Scheduler::setThis();
    set_hook_enable(true);
    if(getThreadId() != m_rootThread) {
        t_scheduler_fiber = Fiber::getThis().get();
    }

    Fiber::Ptr cb_fiber;
    FiberTask ft;
    Fiber::Ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this), 0, false));
    while(true) {
        ft.reset();
        bool needTickle = false;
        bool isActive = false;
        {
            std::lock_guard<std::mutex> lock(m_fibers_mtx);
            auto it = m_fibers.begin();
            while(it != m_fibers.end()) {
                if(it->thread_id != -1 && it->thread_id != getThreadId()) {
                    needTickle = true;
                    ++it;
                    continue;
                }

                Assert((it->fiber || it->cb));
                if(it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                isActive = true;

                ft = *it;
                m_fibers.erase(it);
                ++m_activeThreadNum;
                break;
            }
            needTickle |= (it == m_fibers.end());
        }

        if(needTickle) {
            tickle();
        }

        if(ft.fiber && ft.fiber->getState() != Fiber::TERM 
                    && ft.fiber->getState() != Fiber::EXCEPT) {
            ft.fiber->swapIn();
            --m_activeThreadNum;
            if(ft.fiber->getState() == Fiber::READY) {
                schedule(ft.fiber, ft.thread_id);
            }
            else {
                if(ft.fiber->getState() != Fiber::TERM 
                    && ft.fiber->getState() != Fiber::EXCEPT) {
                        ft.fiber->setState(Fiber::HOLD);
                }
            }
        }
        else if(ft.cb) {
            if(cb_fiber) {
                cb_fiber->reset(ft.cb);
            } else {
                cb_fiber.reset(new Fiber(ft.cb, 0, false));
            }
            int _thread = ft.thread_id;
            ft.reset();
            cb_fiber->swapIn();
            --m_activeThreadNum;
            if(cb_fiber->getState() == Fiber::READY) {
                schedule(cb_fiber, _thread);
                cb_fiber.reset();
            }
            else {
                if(cb_fiber->getState() != Fiber::TERM
                    && cb_fiber->getState() != Fiber::EXCEPT) {
                        cb_fiber->setState(Fiber::HOLD);
                    cb_fiber.reset();
                }
                else {
                    cb_fiber->reset(nullptr);
                    Log_Debug(g_logger) << "scheduler cb terminated " << m_fibers.size();
                }
            }
        }
        else {
            if(isActive) {
                --m_activeThreadNum;
                continue;
            }
            if(idle_fiber->getState() == Fiber::TERM) {
                Log_Debug(g_logger) << "scheduler break";
                break;
            }

            ++m_idelThreadNum;
            // startSleep();
            idle_fiber->swapIn();
            // startWork();
            --m_idelThreadNum;
            if(idle_fiber->getState() != Fiber::TERM 
                && idle_fiber->getState() != Fiber::EXCEPT) {
                    idle_fiber->setState(Fiber::HOLD);
            }
        }
    }
    Log_Debug(g_logger) << "scheduler run finnished";
}

void Scheduler::idle() {
    Log_Info(g_logger) << "idle";
}

void Scheduler::tickle() {
    Log_Info(g_logger) << "tickle";
}

bool Scheduler::stopping() {
    Log_Debug(g_logger) << "Scheduler::stopping " << m_autoStop << ',' << m_running << ',' << m_fibers.empty() << ',' << m_activeThreadNum;
    std::lock_guard<std::mutex> lokc(m_fibers_mtx);
    return m_autoStop && !m_running
        && m_fibers.empty() && (m_activeThreadNum == 0);
}

} // namespace sylar
