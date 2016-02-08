#pragma once

#include <GL/gl3w.h>
#include <string>

// We need to define these our selves since they are extensions ...
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

namespace tankwars {
    inline void* bufferOffset(GLsizeiptr offset) {
        return reinterpret_cast<void*>(offset);
    }

    GLuint createAndCompileShader(const GLchar* source, GLenum type);
    GLuint createShaderFromFile(const std::string& path, GLenum type);
    GLuint createAndLinkProgram(GLuint vertexShader, GLuint fragmentShader);

    GLuint createTextureFromFile(const std::string& path, bool generateMipMaps = true, bool srgb = true);

    bool isExtensionSupported(const std::string& name);
}