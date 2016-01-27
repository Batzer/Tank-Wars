#include "Keyboard.h"

namespace tankwars {
    std::array<bool, Keyboard::NumKeys> Keyboard::keyStates;
    std::array<bool, Keyboard::NumKeys> Keyboard::prevKeyStates;

    bool Keyboard::isKeyDown(int key) {
        return keyStates[key];
    }

    bool Keyboard::isKeyPressed(int key) {
        return keyStates[key] && !prevKeyStates[key];
    }

    bool Keyboard::isKeyReleased(int key) {
        return !keyStates[key] && prevKeyStates[key];
    }

    void Keyboard::init() {
        keyStates.fill(false);
        prevKeyStates.fill(false);
    }

    void Keyboard::update() {
        prevKeyStates = keyStates;
    }

    void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        keyStates[key] = (action != GLFW_RELEASE);
    }

    void Keyboard::windowFocusCallback(GLFWwindow* window, int focused) {
        if (!focused) {
            keyStates.fill(false);
            prevKeyStates.fill(false);
        }
    }
}