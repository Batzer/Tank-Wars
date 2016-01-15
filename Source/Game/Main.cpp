#include <GL/gl3w.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>

#include <btBulletDynamicsCommon.h>

#include "Mesh.h"
#include "MeshTools.h"
#include "Renderer.h"
#include "Terrain.h"
#include "Camera.h"
#include "Game.h"
#include "Physics.h"

#ifdef _DEBUG
constexpr char* WindowTitle = "Tank Wars (Debug)";
constexpr int ResolutionX = 1280;
constexpr int ResolutionY = 720;
constexpr bool GoFullscreen = false;
#else
constexpr char* WindowTitle = "Tank Wars";
constexpr int ResolutionX = 1920;
constexpr int ResolutionY = 1080;
constexpr bool GoFullscreen = true;
#endif

constexpr bool UseVSync = true;
constexpr bool UseMsaa = true;
constexpr double DeltaTime = 1.0 / 60.0;
tankwars::Camera camera(glm::vec3{ -5, 85, 0 }, glm::vec3{ 5, 29, 5 }, glm::vec3{ 0, 1, 0 });
tankwars::Game game(&camera);
void controller();
void render(float alpha);
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
	tankwars::Terrain terrain("Content/Animations/Penis.bmp", 4);
    renderer.setTerrain(&terrain);
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
	renderer.addSceneObject(notASphere);
	/*END OF TRY*/

    tankwars::Physics physics;
    float angle = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = glfwGetTime();
        auto frameTime = currentTime - lastTime;
        lastTime = currentTime;
        accumulator += frameTime;
        
        while (accumulator >= DeltaTime) {
			game.update(static_cast<float>(DeltaTime));
            accumulator -= DeltaTime;
        }
		//if (int(accumulator) % 10000 == 0) {				//THIS SHIT KINDA WORKS BUT THE PERFORMANCE IS A PIECE OF SHIIIIT!
			//terrain.explosionAt(glm::vec3(camera.getCenter().x, 1.5, camera.getCenter().z), 3);
		//}

        // TEST
        angle += frameTime;
        notASphere.transform.rotation = glm::angleAxis(angle, glm::vec3(1, 0, 0));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, ResolutionX, ResolutionY / 2);
        renderer.renderScene(glm::perspective(glm::quarter_pi<float>(), 16.0f / 9, 0.1f, 100.0f) * camera.get());
        glViewport(0, ResolutionY / 2, ResolutionX, ResolutionY / 2);
        renderer.renderScene(glm::perspective(glm::quarter_pi<float>(), 16.0f / 9, 0.1f, 100.0f) * camera.get());

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
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}