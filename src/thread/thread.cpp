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

Thread::Thread(TaskFunc task, const std::string name) {
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
