#include "eventpoller/epManager.h"
#include "util/macro.h"
#include "log/logger.h"

#include <future>

namespace sylar {
static Logger::Ptr g_logger = Name_Logger("system");

Manager::Manager(size_t thread_num, const std::string& name, const size_t load_size) 
    :m_name(name), m_load_size(load_size) {
    Assert((thread_num > 0));

    m_thread_num = thread_num;
    m_lst_assign = 0;
    m_last_pullup = getTimeUsec();
}

Manager::~Manager() {
    Log_Debug(g_logger) << "Manager::~Manager() " << m_root->joinable();
    if(m_root->joinable()) {
        m_root->join();
    }
    {
        std::unique_lock<MutexType> lock(m_threads_mtx);
        for(auto i : m_workers) {
            i->stop();
        }
        for(auto i : m_threads) {
            if(i->joinable()) {
                i->join();
            }
        }
    }
    m_running = false;
}

void Manager::setup() {
    Thread::t_setName(m_name);
    m_rootThreadId = getThreadId();
}

void Manager::start() {
    if(m_running) {
        return;
    }
    m_running = true;
    {
        std::unique_lock<std::shared_mutex> lock(m_threads_mtx);
        m_threads.resize(m_thread_num);
        m_workers.resize(m_thread_num);

        for(size_t i = 0; i < m_thread_num; ++i) {
            std::promise<EventPoller*> prom;
            auto fut = prom.get_future();
            m_threads[i].reset(new Thread([&prom](){
                EventPoller* t = new EventPoller(1);
                prom.set_value(t);
            }, m_name + std::to_string(i+1)));
            m_workers[i].reset(fut.get());
            m_threadIds[i] = m_threads[i]->getId();
        }

        m_root.reset(new Thread([this](){
            this->setup();
            m_sem.notify();
        }, m_name + "0"));
        m_sem.wait();
    }
}

bool Manager::getWorker(size_t& id) {
    auto len = m_thread_num - 1;
    auto min_id = m_lst_assign;
    auto min_load = m_workers[min_id]->getLoad();
    while (len--) {
        auto cur = (m_lst_assign + len) % m_thread_num;
        auto load = m_workers[cur]->getLoad();
        if(load < min_load) {
            min_id = cur;
            min_load = load;
        }
    }
    id = min_id;
    return m_lst_assign == min_id;
}

// #define DEBUG_START

void Manager::run() {
    size_t lst_size = 1;
    while(true) {
#ifndef DEBUG_START
        bool flag = true;
        {
            std::shared_lock<MutexType> read_lock(m_tasks_mtx);
            flag = m_tasks.empty();
        }
        if(!flag) {
            std::unique_lock<MutexType> write_lock(m_tasks_mtx);
            size_t id = m_lst_assign;
            if(getWorker(id)) {
                ++lst_size;
            }
            else {
                lst_size = 1;
            }
            Log_Debug(g_logger) << "get worker: " << id << " : " << m_workers[id]->getLoad() << ',' << m_tasks.size();
            lst_size = std::min(lst_size, m_tasks.size());
            auto len = lst_size;
            while(len--) {
                auto task = m_tasks.front();
                m_tasks.pop_front();
                if(task.cb) {
                    m_workers[id]->schedule(task.cb);
                }
                else if(task.fiber) {
                    m_workers[id]->schedule(task.fiber);
                }
            }
            m_workers[id]->tickle();
        }
        std::vector<int> pullup_ids;
        auto now = getTimeUsec();
        if(now - m_last_pullup >= m_last_pullup) {
            std::shared_lock<MutexType> lock(m_tasks_mtx);
            for(int i = 0; i < m_thread_num; ++i) {
                if(m_threads[i]->isRunning() && !m_threads[i]->joinable()) {
                    pullup_ids.emplace_back(i);
                }
            }
        }
        if(!pullup_ids.empty()) {
            std::unique_lock<MutexType> lock(m_tasks_mtx);
            for(auto i : pullup_ids) {
                m_threadIds.erase(i);
                std::promise<EventPoller*> prom;
                auto fut = prom.get_future();
                m_threads[i].reset(new Thread([&prom](){
                    EventPoller* t = new EventPoller(1);
                    prom.set_value(t);
                }, m_name + std::to_string(i+1)));
                m_workers[i].reset(fut.get());
                m_threadIds[i] = m_threads[i]->getId();
            }
        }
        sleep(1);
#endif //DEBUG_START
    }
    m_running = false;
    Log_Debug(g_logger) << "Manager::run() out";
}

} // namespace sylar
