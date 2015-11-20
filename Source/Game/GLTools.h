#pragma once

#include <GL/gl3w.h>

namespace gp {
    inline void* bufferOffset(GLsizeiptr offset) {
        return reinterpret_cast<void*>(offset);
    }
}