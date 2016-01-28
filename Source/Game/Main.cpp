#include <iostream>
#include <memory>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>

#include "Mesh.h"
#include "MeshInstance.h"
#include "MeshTools.h"
#include "Renderer.h"
#include "Terrain.h"
#include "Camera.h"
#include "Game.h"
#include "Physics.h"
#include "Tank.h"
#include "Wavefront.h"
#include "Keyboard.h"

constexpr char* WindowTitle = "Tank Wars";
constexpr int ResolutionX = 1280;
constexpr int ResolutionY = 720;
constexpr bool GoFullscreen = false;
constexpr bool UseVSync = true;
constexpr bool UseMsaa = false;
constexpr double DeltaTime = 1.0 / 60.0;

tankwars::Tank *tank;

void errorCallback(int error, const char* description) {
    std::cerr << description;
}

int main() {
    // Init glfw
    glfwSetErrorCallback(&errorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW3.\n";
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, UseMsaa ? 4 : 1); // MSAA

    // Create the window and GL context
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(ResolutionX, ResolutionY, WindowTitle,
        GoFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to create a window and context.\n";
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(UseVSync ? 1 : 0); // Turn on VSync

    // Init the OpenGL function loader
    if (gl3wInit()) {
        std::cerr << "Failed to initialize GL3W.\n";
    }

    if (!gl3wIsSupported(3, 3)) {
        std::cerr << "OpenGL 3.3 Core is not supported on this device.\n";
    }

    // Init bullet physics
    std::unique_ptr<btBroadphaseInterface> broadphase = std::make_unique<btDbvtBroadphase>();
    auto collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
    auto dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
    auto solver = std::make_unique<btSequentialImpulseConstraintSolver>();
    auto dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
        dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());

    // Init input systems
    tankwars::Keyboard::init();
    glfwSetKeyCallback(window, &tankwars::Keyboard::keyCallback);
    glfwSetWindowFocusCallback(window, &tankwars::Keyboard::windowFocusCallback);

    // The game loop
    auto lastTime = glfwGetTime();

    // TEST
    tankwars::Renderer renderer;
    tankwars::VoxelTerrain terrain2 = tankwars::VoxelTerrain::fromHeightMap("Content/Maps/test_very_big.png", 16, 8, 16, 16);
    renderer.setTerrain(&terrain2);
    tankwars::Terrain terrain("Content/Maps/Penis.bmp", 2);

    auto tankBodyModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankBody.obj");
    auto tankHeadModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankHead.obj");
    auto tankCanonModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankShootingThing.obj");

    auto tankBodyMesh = tankwars::createMeshFromWavefront(tankBodyModel);
    auto tankHeadMesh = tankwars::createMeshFromWavefront(tankHeadModel);
    auto tankCanonMesh = tankwars::createMeshFromWavefront(tankCanonModel);

    tankwars::Material tankMaterial;
    tankMaterial.diffuseColor = { 0.6f, 0.6f, 0 };
    tankMaterial.specularColor = { 1, 1, 0 };
    tankMaterial.specularExponent = 16;

    auto tankModelMat = glm::translate(glm::mat4(1), glm::vec3(50, 50, -50));
    tankModelMat = glm::scale(tankModelMat, glm::vec3(8, 8, 8));

    tankwars::MeshInstance tankBodyInstance(tankBodyMesh, tankMaterial);
    tankBodyInstance.modelMatrix = tankModelMat;

    tankwars::MeshInstance tankHeadInstance(tankHeadMesh, tankMaterial);
    tankHeadInstance.modelMatrix = tankModelMat;

    tankwars::MeshInstance tankCanonInstance(tankCanonMesh, tankMaterial);
    tankCanonInstance.modelMatrix = tankModelMat;

    renderer.addSceneObject(tankBodyInstance);
    renderer.addSceneObject(tankHeadInstance);
    renderer.addSceneObject(tankCanonInstance);

    float roll = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float camSpeed = 20.0f;
    tankwars::Camera freeCam;
    freeCam.position = { 10, 40, 10 };
    // END TEST

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = glfwGetTime();
        auto frameTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        // Update simulation
        dynamicsWorld->stepSimulation(frameTime, 7);

        // Update game here

        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GL_TRUE);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_W)) freeCam.position += freeCam.direction *  static_cast<float>(frameTime) * camSpeed;
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_S)) freeCam.position -= freeCam.direction *  static_cast<float>(frameTime) * camSpeed;
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_A)) freeCam.position -= freeCam.getRight() *  static_cast<float>(frameTime) * camSpeed;
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_D)) freeCam.position += freeCam.getRight() *  static_cast<float>(frameTime) * camSpeed;
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_UP)) roll += glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_DOWN)) roll -= glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_LEFT)) yaw += glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_RIGHT)) yaw -= glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        freeCam.setAxes(glm::quat({ roll, yaw, 0 }));
        freeCam.update();

        terrain2.updateMesh();

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, ResolutionX, ResolutionY);
        renderer.renderScene(freeCam.getViewProjMatrix(), freeCam.position);

        glfwSwapBuffers(window);

        // Update events and input
        glfwPollEvents();
        tankwars::Keyboard::update();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}