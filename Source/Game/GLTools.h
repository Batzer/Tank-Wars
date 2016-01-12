#pragma once

#include <GL/gl3w.h>
#include <string>

namespace tankwars {
    inline void* bufferOffset(GLsizeiptr offset) {
        return reinterpret_cast<void*>(offset);
    }

    GLuint createAndCompileShader(const GLchar* source, GLenum type);
    GLuint createShaderFromFile(const std::string& path, GLenum type);
    GLuint createAndLinkProgram(GLuint vertexShader, GLuint fragmentShader);
}