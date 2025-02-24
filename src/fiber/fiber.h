#ifndef _SYLAR_FIBER_H_
#define _SYLAR_FIBER_H_

#include <memory>
#include <functional>
#include <ucontext.h>

namespace sylar {

class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    using Ptr = std::shared_ptr<Fiber>;
    using FuncType = std::function<void()>;

    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };

private:
    Fiber();

public:
    // 栈大小传0表示使用默认大小
    Fiber(FuncType cb, size_t stacksize = 0, bool use_caller = true);

    ~Fiber();

    ucontext_t getContext() { return std::move(m_ctx); }

    void reset(FuncType cb);

    void swapIn();

    void swapOut();

    void call();

    void back();
public:
    State getState() const { return m_state; }

    uint64_t getId() { return m_id; }

    void setState(State _state) { m_state = _state; }
public:
    static void setThis(Fiber* _f);

    // 若当前线程没有协程则自动创建主协程
    static Fiber::Ptr getThis();

    static void yieldToReady();

    static void yieldToHold();

    static uint64_t getTot();

    static uint64_t GetFiberId();

    static void MainFunc();

    static void CallerMainFunc();
private:
    uint64_t m_id;

    State m_state;
    
    FuncType m_cb;

    void* m_stack = nullptr;

    uint32_t m_stackSize;
    
    ucontext_t m_ctx;
};

} // namespace sylar


#endif //_SYLAR_FIBER_H_