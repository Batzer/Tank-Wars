#include <GL/gl3w.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <btBulletDynamicsCommon.h>

#include "Mesh.h"
#include "MeshTools.h"
#include "Renderer.h"
#include "Terrain.h"
#include "Camera.h"

constexpr int ResolutionX = 1280;
constexpr int ResolutionY = 720;
constexpr char* WindowTitle = "Tank Wars";
constexpr bool GoFullscreen = false;
constexpr bool UseVSync = true;
constexpr bool UseMsaa = true;
constexpr double DeltaTime = 1.0 / 60.0;
tankwars::Camera Camera(glm::vec3{ -5, 85, 0 }, glm::vec3{ 5, 29, 5 }, glm::vec3{ 0, 1, 0 });
void update(float dt);
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

    // REMOVE
    /*auto planeMesh = tankwars::createPlaneMesh(5, 5);
    tankwars::Material mat;
    mat.diffuseColor = {1, 0, 0};
    tankwars::MeshInstance ground;
    ground.mesh = &planeMesh;
    ground.material = &mat;*/
    tankwars::Renderer renderer;
	/*
    const float heightMap[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 2, 2, 2, 2, 2, 2, 1, 0,
        0, 1, 2, 3, 3, 3, 3, 2, 1, 0,
        0, 1, 2, 3, 4, 5, 3, 2, 1, 0,
        0, 1, 2, 3, 4, 5, 3, 2, 1, 0,
        0, 1, 2, 3, 3, 3, 3, 2, 1, 0,
        0, 1, 2, 2, 2, 2, 2, 2, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    tankwars::Terrain terrain(heightMap, 10, 10);*/
	tankwars::Terrain terrain = tankwars::Terrain("terrain-heightmap.bmp",4);
    renderer.setTerrain(&terrain);
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

    float angle = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = glfwGetTime();
        auto frameTime = currentTime - lastTime;
        lastTime = currentTime;
        accumulator += frameTime;
        
        while (accumulator >= DeltaTime) {
            update(static_cast<float>(DeltaTime));
            accumulator -= DeltaTime;
        }
		/*if (int(accumulator) % 10000 == 0) {				//THIS SHIT KINDA WORKS BUT THE PERFORMANCE IS A PIECE OF SHIIIIT!
			terrain.explosionAt(glm::vec3(Camera.getCenter().x, 1.5, Camera.getCenter().z), 3);
		}*/

        // TEST
        angle += frameTime;
        notASphere.transform.rotation = glm::angleAxis(angle, glm::vec3(1, 0, 0));

        renderer.renderScene(glm::perspective(glm::quarter_pi<float>(), 16.0f / 9, 0.1f, 100.0f) * Camera.get());
        render(static_cast<float>(accumulator / DeltaTime));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void update(float dt) {

}

void render(float alpha) {

}

void errorCallback(int error, const char* description) {
    std::cerr << description;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
	if (key == GLFW_KEY_Q) {
		Camera.rotate(0.2);//0.05
	}
	if (key == GLFW_KEY_E) {
		Camera.rotate(-0.2);
	}
	float alpha = 1;
	if (key == GLFW_KEY_W) {
		Camera.move(0,alpha);
	}
	if (key == GLFW_KEY_S) {
		Camera.move(1,alpha);
	}
	if (key == GLFW_KEY_A) {
		Camera.move(2,alpha);
	}
	if (key == GLFW_KEY_D) {
		Camera.move(3,alpha);
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