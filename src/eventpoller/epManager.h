#ifndef _SYLAR_EPMANAGER_H_
#define _SYLAR_EPMANAGER_H_

#include "eventpoller/eventpoller.h"

namespace sylar {
    
class Manager : public std::enable_shared_from_this<Manager> {
    public:
        using Ptr = std::shared_ptr<Manager>;
        using MutexType = std::shared_mutex;
    public:
        Manager(size_t thread_num, const std::string& name = "test", const size_t load_size = 10);
    
        ~Manager();
    
        template<class T>
        void assignTask(T cb, int thread = -1) {
            distributeTask(cb, thread);
        }
    
        template<class Iterator>
        void assignTask(Iterator begin, Iterator end) {
            while(begin != end) {
                distributeTask(&(*begin));
                ++begin;
            }
        }
    
        void start();
    protected:
        // 负责线程监管与任务分发
        void run();
    
        void setup();
    
        bool getWorker(size_t& id);
    
        template<class T>
        void distributeTask(T cb, int thread = -1){
            auto task = FiberTask(cb, thread);
            if(task.cb || task.fiber) {
                std::unique_lock<MutexType> lock(m_tasks_mtx);
                m_tasks.emplace_back(task);
            }
        }
    
    private:
        bool m_running = false;
        bool m_autoStop = false;
        
        size_t m_load_size = 10;
        size_t m_lst_assign = 1;
        size_t m_thread_num = 1;
        size_t m_idelThread_num = 0;
        size_t m_activeThread_num = 0;
    
        uint64_t m_pullup_dur;
        uint64_t m_last_pullup;
    
        semaphore m_sem;
    
        std::string m_name;
    
        Thread::Ptr m_root;
        int m_rootThreadId;
        Fiber::Ptr m_mainFiber;
    
        MutexType m_ids_mtx;
        std::unordered_map<int,int> m_threadIds;
    
        MutexType m_threads_mtx;
        std::vector<Thread::Ptr> m_threads; // 第0个是root thread
        std::vector<EventPoller::Ptr> m_workers;
    
        MutexType m_tasks_mtx;
        std::list<FiberTask> m_tasks;
};

} // namespace sylar 
#endif //_SYLAR_EPMANAGER_H_