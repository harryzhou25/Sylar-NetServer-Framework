#include "util.h"

namespace sylar {

pid_t getThreadId() {
    return syscall(SYS_gettid);
}

uint32_t getFiberId() {
    return 0;
}

} // namespace sylar
