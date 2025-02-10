#ifndef _SYLAR_THREAD_H
#define _SYLAR_THREAD_H

#include <memory>
#include <thread>
#include <pthread.h>

#include "util/util.h"
#include "thread/mutex.h"

namespace sylar {

// 主模板
template<class R, class... ArgTypes>
class TaskBody;

template<class R, class... ArgTypes>
class TaskBody<R(ArgTypes...)> {
public:
    using Ptr = std::shared_ptr<TaskBody>;
    using func_type = std::function<R(ArgTypes...)>;

    TaskBody() = default;
    ~TaskBody() = default;

    template<typename FUNC>
    TaskBody(FUNC &&task){
        _strongTask = std::make_shared<func_type>(std::forward<FUNC>(task));
        _weakTask = _strongTask;
    }

    operator bool() { return _strongTask && *_strongTask; }

    void operator=(std::nullptr_t) { _strongTask = nullptr; }

    R operator()(ArgTypes ...args) const {
        auto strongTask = _weakTask.lock();
        if (strongTask && *strongTask) {
            return (*strongTask)(std::forward<ArgTypes>(args)...);
        }
        return defaultValue<R>();
    }

    template<typename T>
    static typename std::enable_if<std::is_void<T>::value, void>::type
    defaultValue() {}

    template<typename T>
    static typename std::enable_if<std::is_pointer<T>::value, T>::type
    defaultValue() {
        return nullptr;
    }

    template<typename T>
    static typename std::enable_if<std::is_integral<T>::value, T>::type
    defaultValue() {
        return 0;
    }

protected:
    std::weak_ptr<func_type> _weakTask;
    std::shared_ptr<func_type> _strongTask;
};

using Task = TaskBody<void()>;
using TaskFunc = std::function<void()>;

class Thread : public noncopyable {
public:
    using Ptr = std::shared_ptr<Thread>;

    Thread(TaskFunc task, const std::string name);

    ~Thread();

    int getId() const { return m_id; }

    const std::string& getName() const { return std::move(m_name); }

    void join();

    static std::weak_ptr<std::thread> t_getThis();

    static const std::string& t_getName();

    static void t_setName(const std::string& name);
private:
    void run();

    void setup();
private:
    int m_id = -1;
    Task::Ptr m_task;
    std::string m_name;
    semaphore m_sem;
    bool m_running = false;
    std::shared_ptr<std::thread> m_thread;
};

}

#endif //_SYLAR_THREAD_H