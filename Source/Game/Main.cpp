#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <memory>

GLuint CreateAndCompileShader(const GLchar* source, GLenum type)
{
    auto shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::unique_ptr<GLchar[]> infoLogBuffer(new GLchar[infoLogLength]);
        glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLogBuffer.get());
        glDeleteShader(shader);

        std::string errorMessage("Failed to compile shader with error message:\n");
        errorMessage += infoLogBuffer.get();
        throw std::runtime_error(errorMessage);
    }

    return shader;
}

GLuint CreateShaderFromFile(const std::string& path, GLenum type)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader-file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return CreateAndCompileShader(buffer.str().c_str(), type);
}

GLuint CreateAndLinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus)
    {
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

    if (!validateStatus)
    {
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

/*
* Gets called by GLFW3 when an error occurs.
*/
void ErrorCallback(int error, const char* description)
{
    (void)error;

    std::cerr << description;
}

/*
* Gets called by GLFW3 when a keyboard key is pressed or released.
*/
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main()
{
    glfwSetErrorCallback(&ErrorCallback);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW3.\n";
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "The Game", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create a window and context.\n";
    }

    glfwSetKeyCallback(window, &KeyCallback);
    glfwMakeContextCurrent(window);

    if (gl3wInit())
    {
        std::cerr << "Failed to initialize GL3W.\n";
    }

    if (!gl3wIsSupported(3, 3))
    {
        std::cerr << "OpenGL 3.3 Core is not supported on this device.\n";
    }

    // Create some opengl stuff
    auto vertexShader = CreateShaderFromFile("Content/Shaders/Basic.vsh", GL_VERTEX_SHADER);
    auto fragmentShader = CreateShaderFromFile("Content/Shaders/Basic.fsh", GL_FRAGMENT_SHADER);
    auto shaderProgram = CreateAndLinkProgram(vertexShader, fragmentShader);

    const GLfloat vertices[] =
    {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, 0);
    glBindVertexArray(0);

    glfwSwapInterval(1); // Turn on VSync
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}