#include "thread/thread.h"
#include "log/logger.h"

#include <iostream>

namespace sylar {

static Logger::Ptr g_logger = Name_Logger("system");
static thread_local std::string t_thread_name = "UNKNOWN";
static thread_local std::weak_ptr<std::thread> t_thread;

std::weak_ptr<std::thread> Thread::t_getThis() {
    return t_thread;
}

const std::string& Thread::t_getName() {
    return t_thread_name;
}

static uint64_t getTimeUsec() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

LoadCounter::LoadCounter(size_t max_size) 
    :m_max_size(max_size) {
    m_last_sleep = m_last_wake = getTimeUsec();
}

void LoadCounter::startSleep() {
    std::unique_lock<MutexType> lock(m_mtx);
    m_sleeping = true;
    auto current_time = getTimeUsec();
    auto run_time = current_time - m_last_wake;
    m_last_sleep = current_time;
    m_records.emplace_back(run_time, false);
    if (m_records.size() > m_max_size) {
        m_records.pop_front();
    }
}

void LoadCounter::startWork() {
    std::unique_lock<MutexType> lock(m_mtx);
    m_sleeping = false;
    auto current_time = getTimeUsec();
    auto sleep_time = current_time - m_last_sleep;
    m_last_wake = current_time;
    m_records.emplace_back(sleep_time, true);
    if (m_records.size() > m_max_size) {
        m_records.pop_front();
    }
}

int LoadCounter::getLoad() {
    std::shared_lock<MutexType> lock(m_mtx);
    uint64_t totalSleepTime = 0;
    uint64_t totalRunTime = 0;
    for(auto i : m_records) {
        if(i.sleep) {
            totalSleepTime += i.duration;
        }
        else {
            totalRunTime += i.duration;
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

void Thread::t_setName(const std::string& name) {
    if(name.empty()) {
        return;
    }
    t_thread_name = name;
}

void Thread::join() {
    // Log_Info(g_logger) << "Thread::join " << m_id;
    if(m_thread->joinable()) {
        m_thread->join();
    }
    m_running = false;
}

Thread::Thread(TaskFunc task, const std::string name, size_t load_size)
    :LoadCounter(load_size) {
    m_name = name.empty() ? "UNKNOWN" : name.substr(0, 15);
    m_task = std::make_shared<Task>(std::forward<TaskFunc>(task));

    m_thread = std::make_shared<std::thread>([this] {
        m_running = true;
        run();
    });

    m_sem.wait();
}

Thread::~Thread() {
    Log_Info(g_logger) << "Thread::~Thread " << m_id;
    if(m_thread && m_running) {
        m_thread->detach();
    }
}

void Thread::setup() {
    t_thread = m_thread;
    m_id = getThreadId();
    pthread_setname_np(pthread_self(), m_name.c_str());
    t_thread_name = m_name;
}

void Thread::run() {
    setup();
    m_sem.notify();
    (*m_task)();
}

} // namespace sylar
