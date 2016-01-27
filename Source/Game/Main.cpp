#include <GL/gl3w.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <memory>
#include <array>

#include <btBulletDynamicsCommon.h>

#include "Mesh.h"
#include "MeshTools.h"
#include "Renderer.h"
#include "Terrain.h"
#include "Camera.h"
#include "Game.h"
#include "Physics.h"
#include "Tank.h"

constexpr char* WindowTitle = "Tank Wars";
constexpr int ResolutionX = 1280;
constexpr int ResolutionY = 720;
constexpr bool GoFullscreen = false;
constexpr bool UseVSync = true;
constexpr bool UseMsaa = false;
constexpr double DeltaTime = 1.0 / 60.0;

tankwars::Camera camera(glm::vec3{ -5, 85, 0 }, glm::vec3{ 5, 29, 5 }, glm::vec3{ 0, 1, 0 });
tankwars::Camera camera2(glm::vec3{ -5, 85, 0 }, glm::vec3{ 5, 29, 5 }, glm::vec3{ 0, 1, 0 });
tankwars::Game game(&camera);
std::array<bool, GLFW_KEY_LAST> keyStates;

tankwars::Tank *tank;

void errorCallback(int error, const char* description);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

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

    glfwSetKeyCallback(window, &keyCallback);
    glfwSetFramebufferSizeCallback(window, &framebufferSizeCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(UseVSync ? 1 : 0); // Turn on VSync

    // Init the OpenGL function loader
    if (gl3wInit()) {
        std::cerr << "Failed to initialize GL3W.\n";
    }

    if (!gl3wIsSupported(3, 3)) {
        std::cerr << "OpenGL 3.3 Core is not supported on this device.\n";
    }
    // The game loop
    auto lastTime = glfwGetTime();
    double accumulator = 0.0;
    
    tankwars::Renderer renderer;
    tankwars::VoxelTerrain terrain2 = tankwars::VoxelTerrain::fromHeightMap("Content/Maps/test_big.png", 16, 8, 16, 16);
    renderer.setTerrain(&terrain2);
	tankwars::Terrain terrain("Content/Maps/Penis.bmp", 2);

	//GAME SETUP
	game.setupControllers();
	game.addTerrain(&terrain);

	/*TRYING TO DRAW A CUTE SPHERE*/
	auto boxMesh = tankwars::createBoxMesh(1, 1, 1);
	tankwars::Material mat;
	mat.diffuseColor = { 1,0,0 };
	// tankwars::Transform trans;
	tankwars::MeshInstance notASphere;
    notASphere.mesh = &boxMesh;
    notASphere.material = &mat;
	//renderer.addSceneObject(notASphere);
	/*END OF TRY*/

    tankwars::Physics physics;
	tankwars::Tank tank1(physics.getDynamicsWorld(), btVector3(20, 20, -20));
	for (int i = 0; i < 7; i++) {
		renderer.addSceneObject(tank1.getTankMeshInstance(i));
	}
	tank = &tank1;
    float angle = 0.0f;
    int bla = 0;

    glm::vec3 camPos(10, 40, 10);
    glm::vec3 camDir(0, 0, -1);
    glm::vec3 camRight(1, 0, 0);
    glm::vec3 camUp(0, 1, 0);
    float roll = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float camSpeed = 20.0f;

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = glfwGetTime();
        auto frameTime = currentTime - lastTime;
        lastTime = currentTime;
        accumulator += frameTime;
		tank1.update();
        while (accumulator >= DeltaTime) {
			physics.getDynamicsWorld()->stepSimulation(DeltaTime);
			game.update(static_cast<float>(DeltaTime));
            accumulator -= DeltaTime;
        }
		tank1.renderme();
        if (keyStates[GLFW_KEY_W]) camPos += camDir *  static_cast<float>(frameTime) * camSpeed;
        if (keyStates[GLFW_KEY_S]) camPos -= camDir *  static_cast<float>(frameTime) * camSpeed;
        if (keyStates[GLFW_KEY_A]) camPos -= camRight *  static_cast<float>(frameTime) * camSpeed;
        if (keyStates[GLFW_KEY_D]) camPos += camRight *  static_cast<float>(frameTime) * camSpeed;
        if (keyStates[GLFW_KEY_UP]) roll += glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (keyStates[GLFW_KEY_DOWN]) roll -= glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (keyStates[GLFW_KEY_LEFT]) yaw += glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (keyStates[GLFW_KEY_RIGHT]) yaw -= glm::quarter_pi<float>() *  static_cast<float>(frameTime);

        
       /* bla++;
        if (bla % 15 == 0) {
            for (size_t z = 0; z < terrain2.getDepth(); z++)
            for (size_t y = 0; y < terrain2.getHeight(); y++)
            for (size_t x = 0; x < terrain2.getWidth(); x++) {
                if (x == 0 || y == 0 || z == 0 || x == terrain2.getWidth() - 1
                        || y == terrain2.getHeight() - 1 || z == terrain2.getDepth() - 1) {
                    terrain2.setVoxel(x, y, z, rand() % 2 == 0 ? tankwars::VoxelType::Solid : tankwars::VoxelType::Empty);
                }
                else {
                    terrain2.setVoxel(x, y, z, tankwars::VoxelType::Solid);
                }
            }
        }*/
		
        terrain2.updateMesh();

		
        auto rotation = glm::angleAxis(yaw, glm::vec3(0, 1, 0))
            * glm::angleAxis(roll, glm::vec3(1, 0, 0))
            * glm::angleAxis(pitch, glm::vec3(0, 0, 1));
        camDir = glm::rotate(rotation, glm::vec3(0, 0, -1));
        camRight = glm::rotate(rotation, glm::vec3(1, 0, 0));
        camUp = glm::rotate(rotation, glm::vec3(0, 1, 0));
		glm::tmat4x4<float> viewMat;
		if (keyStates[GLFW_KEY_R]) {
			glm::vec3 pos= tank1.getPosition();
			viewMat = glm::lookAt(camPos, pos, camUp);
		}
		else {
			viewMat = glm::lookAt(camPos, camPos + camDir, camUp);
		}

        // TEST
        angle += static_cast<float>(frameTime);
        notASphere.transform.rotation = glm::angleAxis(angle, glm::vec3(1, 0, 0));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /*
        glViewport(ResolutionX/5, 0, ResolutionX, ResolutionY / 2);
        renderer.renderScene(glm::perspective(glm::quarter_pi<float>(), 16.0f / 9, 0.1f, 100.0f) * camera2.getViewMatrix());
        glViewport(0, ResolutionY / 2, ResolutionX/2+ResolutionX/5, ResolutionY / 2);
        */

        glViewport(0, 0, ResolutionX, ResolutionY);
        renderer.renderScene(glm::perspective(glm::quarter_pi<float>(), 16.0f / 9, 1.0f, 500.0f) * viewMat, camPos);

		game.render(static_cast<float>(accumulator / DeltaTime));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



void errorCallback(int error, const char* description) {
    std::cerr << description;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
	if (key == GLFW_KEY_Q) {
		camera.rotateXAxis(0.2);//0.05
	}
	if (key == GLFW_KEY_E) {
		camera.rotateXAxis(-0.2);
	}
	float alpha = 1;
	if (key == GLFW_KEY_W) {
		camera.move(0,alpha);
	}
	if (key == GLFW_KEY_S) {
		camera.move(1,alpha);
	}
	if (key == GLFW_KEY_A) {
		camera.move(2,alpha);
	}
	if (key == GLFW_KEY_D) {
		camera.move(3,alpha);
	}
	if (key == GLFW_KEY_Y) {	//move up

	}
	if (key == GLFW_KEY_X) {	//move down

	}
	if (key == GLFW_KEY_SPACE) {
		//terrain.explosionAt(glm::vec3(Camera.getCenter().x, 0, Camera.getCenter().z), 20);
	}
	if (key == GLFW_KEY_J) {
		tank->turn(true);
	}
	if (key == GLFW_KEY_L) {
		tank->turn(false);
	}
	if (key == GLFW_KEY_I) {
		tank->drive(true);
	}
	if (key == GLFW_KEY_K) {
		tank->drive(false);
	}

    keyStates[key] = (action != GLFW_RELEASE);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}