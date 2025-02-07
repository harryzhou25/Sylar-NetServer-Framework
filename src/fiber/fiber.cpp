#include "fiber/fiber.h"
#include "config/config.h"
#include "util/macro.h"
#include "log/logger.h"

namespace sylar {

static Logger::Ptr g_logger = SYLAR_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::Ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
    Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

class StackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};

Fiber::Fiber() {
    m_state = EXEC;

    m_id = ++s_fiber_id;
    ++s_fiber_count;

    m_stackSize = g_fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stackSize);
    
    if(getcontext(&m_ctx)) {
        Assert_Commit(false, "Fibier get context failed");
    }

    setThis(this);

    Log_Debug(g_logger) << "Main Fiber created";
}

Fiber::Fiber(FuncType cb, size_t stacksize) {
    m_state = INIT;

    m_id = ++s_fiber_id;
    ++s_fiber_count;

    m_cb = std::forward<FuncType>(cb);

    m_stackSize = stacksize ? stacksize : g_fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stackSize);

    if(getcontext(&m_ctx)) {
        Assert_Commit(false, "Fibier get context failed");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stackSize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);

    Log_Debug(g_logger) << "Created Fiber: " << m_id;
}

Fiber::~Fiber() {
    --s_fiber_count;
    if(m_stack) {
        Assert((m_state == INIT || m_state == TERM));
        StackAllocator::Dealloc(m_stack, m_stackSize);
    }
    else {
        Assert(!m_cb);
        Assert((m_state == EXEC));
        auto cur = t_fiber;
        if(cur == this) {
            setThis(nullptr);
        }
    }
    Log_Debug(g_logger) << "Destoried Fiber: " << m_id;
}

void Fiber::reset(FuncType cb) {
    Assert(m_stack);
    Assert((m_state == INIT || m_state == TERM || m_state == EXCEPT));

    m_cb = std::forward<FuncType>(cb);
    
    if(getcontext(&m_ctx)) {
        Assert_Commit(false, "Fibier get context failed");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stackSize;

    m_state = INIT;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);

    Log_Debug(g_logger) << "Fiber " << m_id << " reseted";    
}

Fiber::Ptr Fiber::getThis() {
    if(t_fiber) {
        return t_fiber->shared_from_this();
    }
    // 没创建主协程
    Fiber::Ptr tmp(new Fiber);
    Assert((t_fiber == tmp.get()));
    t_threadFiber = tmp;
    return t_fiber->shared_from_this();
}


void Fiber::swapIn() {
    setThis(this);
    Assert((m_state == EXEC));
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->getContext(), &m_ctx)) {
        Assert_Commit(false, "Fiber swap failed");
    }
}

void Fiber::swapOut() {
    setThis(t_threadFiber.get());
    if(swapcontext(&m_ctx, &t_threadFiber->getContext())) {
        Assert_Commit(false, "Fiber swap failed");
    }
}

void Fiber::yieldToHold() {
    auto cur = getThis();
    Assert((cur->m_state == EXEC));
    cur->m_state = HOLD;
    cur->swapOut();
}

void Fiber::yieldToReady() {
    auto cur = getThis();
    Assert((cur->m_state == EXEC));
    cur->m_state = READY;
    cur->swapIn();
}

uint64_t Fiber::getTot() {
    return s_fiber_count;
}

uint64_t Fiber::GetFiberId() {
    if(t_fiber) {
        return t_fiber->m_id;
    }
    else return 0;
}


void Fiber::MainFunc() {
    auto cur = getThis();
    Assert(cur);

    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }
    catch (std::exception &ex) {
        cur->m_state = EXCEPT;
        Log_Debug(g_logger) << "Fiber " << cur->m_id << " caught exception: " << ex.what();
    }
    catch (...) {
        cur->m_state = EXCEPT;
        Log_Debug(g_logger) << "Fiber " << cur->m_id << " caught exception";
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();
}

} // namespace sylar
