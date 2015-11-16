#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

void ErrorCallback(int error, const char* description)
{
    (void)error;

    std::cerr << description;
}

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

    glfwSwapInterval(1); // Turn on VSync
    glClearColor(0.0f, 0.0f, 0.4f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}