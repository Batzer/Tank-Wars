#pragma once

#include <array>

#include <GL/gl3w.h> // To be sure
#include <GLFW/glfw3.h>

namespace tankwars {
    class Keyboard {
    public:
        Keyboard() = delete;

        // Returns if the key is down or released in the current frame
        static bool isKeyDown(int key);

        // Returns if the key is down this frame, but was up last frame
        static bool isKeyPressed(int key);

        // Returns if the key is up this frame, but was down last frame
        static bool isKeyReleased(int key);

        // Call at startup
        static void init();

        // Call this once per frame (don't call manually!)
        static void update();

        // The callbacks for GLFW events (don't call manually!)
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void windowFocusCallback(GLFWwindow* window, int focused);

    private:
        static constexpr auto NumKeys = GLFW_KEY_LAST + 1;
        static std::array<bool, NumKeys> keyStates;
        static std::array<bool, NumKeys> prevKeyStates;
    };
}