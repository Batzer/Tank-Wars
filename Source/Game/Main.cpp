#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

/*
* Gets called everytime the framebuffer resizes.
*/
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    (void)window;

    glViewport(0, 0, width, height);
}

int main()
{
    glfwSetErrorCallback(&ErrorCallback);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW3.\n";
    }

    // We need a OpenGL 3.3 Core context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA

    // Window settings
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "The Game", nullptr, nullptr);

    if (!window)
    {
        std::cerr << "Failed to create a window and context.\n";
    }

    glfwSetKeyCallback(window, &KeyCallback);
    glfwSetFramebufferSizeCallback(window, &FramebufferSizeCallback);
    glfwMakeContextCurrent(window);

    if (gl3wInit())
    {
        std::cerr << "Failed to initialize GL3W.\n";
    }

    if (!gl3wIsSupported(3, 3))
    {
        std::cerr << "OpenGL 3.3 Core is not supported on this device.\n";
    }

    // Output some system information to the standard output
    std::cout << "OpenGL Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << "\n";
    std::cout << "GLSL Version:   " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)) << "\n";
    std::cout << "Renderer:       " << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << "\n";

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

    auto modelMatrixLocation = glGetUniformLocation(shaderProgram, "ModelMatrix");

    glfwSwapInterval(1); // Turn on VSync
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    float angle = 0.0f;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        auto currTime = glfwGetTime();
        auto dt = static_cast<float>(currTime - lastTime);
        lastTime = currTime;

        angle += glm::half_pi<float>() * dt;
        auto modelMatrix = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
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