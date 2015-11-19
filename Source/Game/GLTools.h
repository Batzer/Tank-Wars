#pragma once

#include <cstdint>

namespace gp {
    inline void* bufferOffset(std::uintptr_t offset) {
        return reinterpret_cast<void*>(offset);
    }
}