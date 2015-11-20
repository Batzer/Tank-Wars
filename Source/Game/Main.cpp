#include "StaticMesh.h"
#include "MeshTools.h"
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

    auto modelMatrixLocation = glGetUniformLocation(shaderProgram, "ModelMatrix");
    auto invTrModelMatrixLocation = glGetUniformLocation(shaderProgram, "InvTrModelMatrix");
    auto viewProjMatrixLocation = glGetUniformLocation(shaderProgram, "ViewProjMatrix");
    auto ambientColorLocation = glGetUniformLocation(shaderProgram, "AmbientColor");
    auto lightColorLocation = glGetUniformLocation(shaderProgram, "LightColor");
    auto dirToLightLocation = glGetUniformLocation(shaderProgram, "DirToLight");
    auto materialDiffuseLocation = glGetUniformLocation(shaderProgram, "MaterialDiffuse");

    auto boxMesh = gp::createBoxMesh(1, 1, 1);
    auto planeMesh = gp::createPlane(10, 10);

    glm::vec3 dirToLight(-1, 1, 1);
    dirToLight = glm::normalize(dirToLight);

    glm::vec3 ambientColor(0.2f, 0.2f, 0.2f);
    glm::vec3 materialDiffuse(1.0f, 0.0f, 0.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    glfwSwapInterval(1); // Turn on VSync
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
    glClearDepthf(1.0f);

    // Back face culling, CCW faces are front faces
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);

    float angle = 0.0f;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        auto currTime = glfwGetTime();
        auto dt = static_cast<float>(currTime - lastTime);
        lastTime = currTime;
        
        angle += glm::half_pi<float>() * dt;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        auto projMat = glm::perspective(glm::quarter_pi<float>(), static_cast<float>(width) / height, 0.1f, 100.0f);
        auto viewMat = glm::lookAt(glm::vec3(1, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        auto modelMat = glm::translate(glm::mat4(1), glm::vec3(0, 2, 0));
        modelMat = glm::rotate(modelMat, angle, glm::vec3(0, 0, 1));
        modelMat = glm::rotate(modelMat, angle, glm::vec3(0, 1, 0));
        modelMat = glm::rotate(modelMat, angle, glm::vec3(1, 0, 0));
        auto invTrModelMat = glm::transpose(glm::inverse(modelMat));
        auto viewProjMat = projMat * viewMat;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix4fv(invTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(invTrModelMat));
        glUniformMatrix4fv(viewProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewProjMat));

        glUniform3fv(ambientColorLocation, 1, glm::value_ptr(ambientColor));
        glUniform3fv(lightColorLocation, 1, glm::value_ptr(lightColor));
        glUniform3fv(dirToLightLocation, 1, glm::value_ptr(dirToLight));
        glUniform3fv(materialDiffuseLocation, 1, glm::value_ptr(materialDiffuse));
        
        boxMesh.bindBuffers();
        boxMesh.render();

        glUniform3f(materialDiffuseLocation, 0, 1, 0);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        glUniformMatrix4fv(invTrModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

        planeMesh.bindBuffers();
        planeMesh.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}