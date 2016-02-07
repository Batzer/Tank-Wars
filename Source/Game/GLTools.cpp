#include "GLTools.h"

#include <fstream>
#include <sstream>
#include <memory>
#include <cstring>

#include "Image.h"

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

    GLuint createTextureFromFile(const std::string& path, bool generateMipMaps, bool srgb) {
        static const GLint channelToFormat[] = {
            GL_R8, GL_RG8, GL_RGB8, GL_RGBA8
        };

        Image image(path);
        GLint format = channelToFormat[image.getNumChannels() - 1];
        GLenum textureFormat;
        GLenum textureFormatType = GL_UNSIGNED_BYTE;

        switch (format) {
        case GL_R8:    textureFormat = GL_RED;  break;
        case GL_RG8:   textureFormat = GL_RG;   break;
        case GL_RGB8:  textureFormat = GL_RGB;  break;
        case GL_RGBA8: textureFormat = GL_RGBA; break;
        }

        if (srgb && textureFormat == GL_RGB8) {
            format = GL_SRGB8;
        }
        else if (srgb && textureFormat == GL_RGBA8) {
            format = GL_SRGB8_ALPHA8;
        }

        if (!textureFormat || !textureFormatType) {
            throw std::runtime_error("invalid texture format");
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, image.getWidth(), image.getHeight(), 0,
                     textureFormat, textureFormatType, image.getImage());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (generateMipMaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        return texture;
    }

    bool isExtensionSupported(const std::string& name) {
        int numExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

        for(int i = 0; i < numExtensions; i++) {
            const GLubyte* extStr = glGetStringi(GL_EXTENSIONS, i);
            if (strcmp(reinterpret_cast<const char*>(extStr), name.c_str()) == 0 ){
                return true;
            }
        }

        return false;
    }
}