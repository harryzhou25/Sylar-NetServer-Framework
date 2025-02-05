#include "util.h"

namespace sylar {

pid_t getThreadId() {
    return syscall(SYS_gettid);
}

uint32_t getFiberId() {
    return 0;
}

void filePathCheck(const std::string& filePath) {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path fullPath = currentPath / filePath;

    std::filesystem::path path(fullPath);
    std::string parentPath = path.parent_path().string();
    if (!std::filesystem::exists(parentPath)) {
        std::filesystem::create_directories(parentPath);
    }
}

} // namespace sylar
