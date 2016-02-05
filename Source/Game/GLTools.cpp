#include "GLTools.h"
#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>

namespace tankwars {
    GLuint createAndCompileShader(const GLchar* source, GLenum type) {
        auto shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint compileStatus;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

        if (!compileStatus) {
            GLint infoLogLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

            if (infoLogLength > 0) {
                std::unique_ptr<GLchar[]> infoLogBuffer(new GLchar[infoLogLength]);
                glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLogBuffer.get());
                
                std::string errorMessage("Failed to compile shader with error message:\n");
                errorMessage += infoLogBuffer.get();
                throw std::runtime_error(errorMessage);
            }

            glDeleteShader(shader);
        }

        return shader;
    }

    GLuint createShaderFromFile(const std::string& path, GLenum type) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open shader-file: " + path);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return createAndCompileShader(buffer.str().c_str(), type);
    }

    GLuint createAndLinkProgram(GLuint vertexShader, GLuint fragmentShader) {
        auto program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint linkStatus;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (!linkStatus) {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::unique_ptr<GLchar[]> infoLogBuffer(new GLchar[infoLogLength]);
            glGetProgramInfoLog(program, infoLogLength, nullptr, infoLogBuffer.get());
            glDeleteProgram(program);

            std::string errorMessage("Failed to link program with error message:\n");
            errorMessage += infoLogBuffer.get();
            throw std::runtime_error(errorMessage);
        }

        glValidateProgram(program);

        GLint validateStatus;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus);

        if (!validateStatus) {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

            std::unique_ptr<GLchar[]> infoLogBuffer(new GLchar[infoLogLength]);
            glGetProgramInfoLog(program, infoLogLength, nullptr, infoLogBuffer.get());
            glDeleteProgram(program);

            std::string errorMessage("Failed to validate program with error message:\n");
            errorMessage += infoLogBuffer.get();
            throw std::runtime_error(errorMessage);
        }

        return program;
    }
}
