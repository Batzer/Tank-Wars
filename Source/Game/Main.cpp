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
#include "Camera.h"
#include "Game.h"
#include "Tank.h"
#include "Wavefront.h"
#include "Keyboard.h"
#include "ExplosionHandling.h"
#include "ParticleSystem.h"
#include "GLTools.h"
#include "Hud.h"

constexpr char* WindowTitle = "Tank Wars";
constexpr int ResolutionX = 1280;
constexpr int ResolutionY = 720;
constexpr bool GoFullscreen = false;
constexpr bool UseVSync = true;
constexpr bool UseMsaa = true;
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
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

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
    std::unique_ptr<btBroadphaseInterface> broadphase(new btDbvtBroadphase);
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration(new btDefaultCollisionConfiguration);
    std::unique_ptr<btCollisionDispatcher> dispatcher(new btCollisionDispatcher(collisionConfiguration.get()));
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver(new btSequentialImpulseConstraintSolver);
    std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld(new btDiscreteDynamicsWorld(
        dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get()));

    // Init input systems
    tankwars::Keyboard::init();
    glfwSetKeyCallback(window, &tankwars::Keyboard::keyCallback);
    glfwSetWindowFocusCallback(window, &tankwars::Keyboard::windowFocusCallback);

    // Setup game stuff
    tankwars::Renderer renderer;
    tankwars::VoxelTerrain terrain2 = tankwars::VoxelTerrain::fromHeightMap(
        "Content/Maps/test_very_big.png", dynamicsWorld.get(), 16, 8, 16, 8);
    renderer.setTerrain(&terrain2);
	
    tankwars::Tank tank1(dynamicsWorld.get(),renderer, btVector3(30, 25, -30),0);
	tankwars::Tank tank2(dynamicsWorld.get(), renderer, btVector3(40, 25, -40), 1);
	gContactAddedCallback = tankwars::customCallback;
    
    float roll = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float camSpeed = 20.0f;
    tankwars::Camera freeCam;
    freeCam.aspectRatio = 16 / 4.5f;
    freeCam.position = { 10, 40, 10 };
    renderer.attachCamera(tankwars::Renderer::ViewportTop, freeCam);
	tankwars::Game game(&freeCam, &terrain2);
	tankwars::explosionHandler.reset(new tankwars::ExplosionHandler(
        dynamicsWorld.get(), renderer, terrain2, &tank1, &tank2, &game));
    
    tankwars::Camera freeCam2;
    freeCam2.position = { 15, 40, 10 };
    freeCam2.aspectRatio = 16 / 4.5f;
    renderer.attachCamera(tankwars::Renderer::ViewportBottom, freeCam2);
    renderer.setSplitScreenEnabled(true);

	game.setupControllers();
	game.bindControllerToTank(0, &tank1);
	game.bindControllerToTank(1, &tank2);
	
	GLuint numbers[10];
	numbers[0] = tankwars::createTextureFromFile("Content/Hud/Numbers/Zero.png");
	numbers[1] = tankwars::createTextureFromFile("Content/Hud/Numbers/One.png");
	numbers[2] = tankwars::createTextureFromFile("Content/Hud/Numbers/Two.png");
	numbers[3] = tankwars::createTextureFromFile("Content/Hud/Numbers/Three.png");
	numbers[4] = tankwars::createTextureFromFile("Content/Hud/Numbers/Four.png");
	numbers[5] = tankwars::createTextureFromFile("Content/Hud/Numbers/Five.png");
	numbers[6] = tankwars::createTextureFromFile("Content/Hud/Numbers/Six.png");
	numbers[7] = tankwars::createTextureFromFile("Content/Hud/Numbers/Seven.png");
	numbers[8] = tankwars::createTextureFromFile("Content/Hud/Numbers/Eight.png");
	numbers[9] = tankwars::createTextureFromFile("Content/Hud/Numbers/Nine.png");
	GLuint minus = tankwars::createTextureFromFile("Content/Hud/Numbers/Minus.png");
	GLuint nothing = tankwars::createTextureFromFile("Content/Hud/Numbers/Nothing.png");

	tankwars::HudSprite kmhNumber1;
	kmhNumber1.transparency = 1;
	kmhNumber1.size = { 0.06,0.21 };
	kmhNumber1.position = {-0.71,-0.901};

	tankwars::HudSprite kmhNumber2;
	kmhNumber2.transparency = 1;
	kmhNumber2.size = { 0.06,0.21 };
	kmhNumber2.position = { -0.735,-0.901 };

	tankwars::HudSprite kmhMinus;
	kmhMinus.transparency = 1;
	kmhMinus.size = { 0.06,0.21 };
	kmhMinus.position = { -0.76,-0.901 };

	tankwars::HudSprite kmhNumber1tank2;
	kmhNumber1tank2.transparency = 1;
	kmhNumber1tank2.size = { 0.06,0.21 };
	kmhNumber1tank2.position = { -0.71,-0.901 };

	tankwars::HudSprite kmhNumber2tank2;
	kmhNumber2tank2.transparency = 1;
	kmhNumber2tank2.size = { 0.06,0.21 };
	kmhNumber2tank2.position = { -0.735,-0.901 };

	tankwars::HudSprite kmhMinustank2;
	kmhMinustank2.transparency = 1;
	kmhMinustank2.size = { 0.06,0.21 };
	kmhMinustank2.position = { -0.76,-0.901 };

	tankwars::HudSprite kmh;
	kmh.transparency = 1;
	kmh.texture = tankwars::createTextureFromFile("Content/Hud/Numbers/kmh.png");
	kmh.size = {0.2,0.2};
	kmh.position = { -0.7,-0.9 };

	tankwars::HudSprite number1;
	number1.transparency = 1;
	number1.size = { 0.1,0.35 };
	number1.position = { 0.92,-1.05 };

	tankwars::HudSprite number2;
	number2.transparency = 1;
	number2.size = { 0.1,0.35 };
	number2.position = { 0.88,-1.05 };


	tankwars::HudSprite number1tank2;
	number1tank2.transparency = 1;
	number1tank2.size = { 0.1,0.35 };
	number1tank2.position = { 0.92,0.7 };

	tankwars::HudSprite number2tank2;
	number2tank2.transparency = 1;
	number2tank2.size = { 0.1,0.35 };
	number2tank2.position = { 0.88,0.7 };

	tankwars::HudSprite hudSprite;
	hudSprite.position = { -1.05 , -1.4 };
	hudSprite.transparency = 1;
	hudSprite.size = { 0.3, 1.8 };
	hudSprite.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy.png");

	tankwars::HudSprite hudSprite2;
	hudSprite2.position = { -1.05 , -1.4 };
	hudSprite2.transparency = 1;
	hudSprite2.size = { 0.3, 1.8 };
	hudSprite2.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy2.png");

	tankwars::HudSprite hudSprite3;
	hudSprite3.position = { -1.05, -1.4 };
	hudSprite3.transparency = 1;
	hudSprite3.size = { 0.3, 1.8 };
	hudSprite3.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy3.png");

	tankwars::HudSprite hudSprite32;
	hudSprite32.position = { -1.05, -1.4 };
	hudSprite32.transparency = 1;
	hudSprite32.size = { 0.3, 1.8 };
	hudSprite32.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy3.png");

	tankwars::HudSprite hudSprite4tr;
	hudSprite4tr.position = { -1.05, -1.4 };
	hudSprite4tr.transparency = 0.4;
	hudSprite4tr.size = { 0.3, 1.8 };
	hudSprite4tr.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy4.png");

	tankwars::HudSprite hudSprite4;
	hudSprite4.position = { -1.05 , -1.4 };
	hudSprite4.transparency = 1;
	hudSprite4.size = { 0.3, 1.8 };
	hudSprite4.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy4.png");

	tankwars::HudSprite hudSprite42;
	hudSprite42.position = { -1.05 , -1.4 };
	hudSprite42.transparency = 1;
	hudSprite42.size = { 0.3, 1.8 };
	hudSprite42.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy4.png");

	tankwars::Hud hud1;
	hud1.addSprite(kmhMinus, 10);
	hud1.addSprite(kmhNumber2, 9);
	hud1.addSprite(kmhNumber1, 8);
	hud1.addSprite(kmh, 7);
	hud1.addSprite(number2, 6);
	hud1.addSprite(number1, 5);
	hud1.addSprite(hudSprite, 4);
	hud1.addSprite(hudSprite2, 3);
	hud1.addSprite(hudSprite3, 0);
	hud1.addSprite(hudSprite4tr, 1);
	hud1.addSprite(hudSprite4, 2);
	renderer.attachHud(tankwars::Renderer::ViewportTop, hud1);

	tankwars::Hud hud2;
	hud2.addSprite(kmhMinustank2, 10);
	hud2.addSprite(kmhNumber2tank2, 9);
	hud2.addSprite(kmhNumber1tank2, 8);
	hud2.addSprite(kmh, 7);
	hud2.addSprite(number2tank2, 6);
	hud2.addSprite(number1tank2, 5);
	hud2.addSprite(hudSprite, 4);
	hud2.addSprite(hudSprite2, 3);
	hud2.addSprite(hudSprite32, 0);
	hud2.addSprite(hudSprite4tr, 1);
	hud2.addSprite(hudSprite42, 2);
	renderer.attachHud(tankwars::Renderer::ViewportBottom, hud2);
	
    // The game loop
    auto lastTime = glfwGetTime();
	int i = 0;
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = glfwGetTime();
        auto frameTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        // Update simulation
        dynamicsWorld->stepSimulation(frameTime, 15, 1.0f / 120.0f);

        // Update game here
		game.update((float)currentTime);

		
		hudSprite4.texSize[1] = 0.2923 + tank1.getShootingPowerInSteps()*0.002226;
		hudSprite4.size[1] = 0.5261 + tank1.getShootingPowerInSteps()*0.0040095;

		hudSprite42.texSize[1] = 0.2923 + tank2.getShootingPowerInSteps()*0.002226;
		hudSprite42.size[1] = 0.5261 + tank2.getShootingPowerInSteps()*0.0040095;

		hudSprite3.texSize[1] = 0.34 + tank1.getShootingTimerRestInSteps((float)currentTime)*0.0024;
		hudSprite3.size[1] = 0.612 + tank1.getShootingTimerRestInSteps((float)currentTime)*0.00432;

		hudSprite32.texSize[1] = 0.34 + tank2.getShootingTimerRestInSteps((float)currentTime)*0.0024;
		hudSprite32.size[1] = 0.612 + tank2.getShootingTimerRestInSteps((float)currentTime)*0.00432;
		
		number1.texture = numbers[tank1.getPoints() % 10];
		number2.texture = numbers[((int)(tank1.getPoints() / 10)) % 10];

		number1tank2.texture = numbers[tank2.getPoints() % 10];
		number2tank2.texture = numbers[((int)(tank2.getPoints() / 10)) % 10];

		kmhNumber1.texture = numbers[std::abs(tank1.getSpeed())%10];
		kmhNumber2.texture = numbers[((int)(std::abs(tank1.getSpeed())/10)) % 10];

		if (tank1.getSpeed() < 0) {
			kmhMinus.texture = minus;
		}
		else {
			kmhMinus.texture = nothing;
		}

		kmhNumber1tank2.texture = numbers[std::abs(tank2.getSpeed()) % 10];
		kmhNumber2tank2.texture = numbers[((int)(std::abs(tank2.getSpeed()) / 10)) % 10];

		if (tank2.getSpeed() < 0) {
			kmhMinustank2.texture = minus;
		}
		else {
			kmhMinustank2.texture = nothing;
		}
		if (tankwars::Keyboard::isKeyDown(GLFW_KEY_R)) {
			i = (i + 1) % 10;
			number2.texture = numbers[i];
		}
		if (tankwars::Keyboard::isKeyDown(GLFW_KEY_F)) {
			hudSprite3.texSize[1] += 0.01;
			hudSprite3.size[1] += 0.018;
		}
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, GL_TRUE);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_W)) freeCam.position += freeCam.direction *  static_cast<float>(frameTime) * camSpeed;
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_S)) freeCam.position -= freeCam.direction *  static_cast<float>(frameTime) * camSpeed;
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_A)) freeCam.position -= freeCam.getRight() *  static_cast<float>(frameTime) * camSpeed;
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_D)) freeCam.position += freeCam.getRight() *  static_cast<float>(frameTime) * camSpeed;
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_UP)) roll += glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_DOWN)) roll -= glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_LEFT)) yaw += glm::quarter_pi<float>() *  static_cast<float>(frameTime);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_RIGHT)) yaw -= glm::quarter_pi<float>() *  static_cast<float>(frameTime);
		if (tankwars::Keyboard::isKeyDown(GLFW_KEY_I))tank1.drive(true);
		if (tankwars::Keyboard::isKeyDown(GLFW_KEY_K)) { tank1.drive(false); tank1.driveBack(false); }
	    if (tankwars::Keyboard::isKeyDown(GLFW_KEY_O)) tank1.driveBack(true);
		if (tankwars::Keyboard::isKeyDown(GLFW_KEY_J))tank1.turn(true);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_L))tank1.turn(false);
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_SPACE)) {
            for (size_t z = 1; z < terrain2.getDepth()-1; z++)
            for (size_t y = 1; y < terrain2.getHeight()-1; y++)
            for (size_t x = 1; x < terrain2.getWidth()-1; x++) {
                terrain2.setVoxel(x, y, z, tankwars::VoxelType::Empty);
            }
            terrain2.updateMesh();
        }
        if (tankwars::Keyboard::isKeyDown(GLFW_KEY_V)) {
            for (size_t z = 0; z < terrain2.getDepth(); z++)
            for (size_t y = 0; y < terrain2.getHeight(); y++)
            for (size_t x = 0; x < terrain2.getWidth(); x++) {
                terrain2.setVoxel(x, y, z, tankwars::VoxelType::Solid);
            }
            terrain2.updateMesh();
        }
        if (tankwars::Keyboard::isKeyPressed(GLFW_KEY_F1)) {
            renderer.setSplitScreenEnabled(false);
            freeCam.aspectRatio = 16.0f / 9.0f;
        }
        if (tankwars::Keyboard::isKeyPressed(GLFW_KEY_F2)) {
            renderer.setSplitScreenEnabled(true);
            freeCam.aspectRatio = 16.0f / 4.5f;
        }
		
		freeCam2.position = (tank2.getPosition() + glm::normalize(-tank2.getDirectionVector())*tank2.getCameraOffsetDistance() + glm::vec3(0, tank2.getCameraOffsetHeight(), 0));
        freeCam2.lookAt(tank2.getPosition() + glm::vec3(0,3,0), { 0,1,0 });

        freeCam2.update();

		freeCam.position = (tank1.getPosition() + glm::normalize(-tank1.getDirectionVector())*tank1.getCameraOffsetDistance() + glm::vec3(0, tank1.getCameraOffsetHeight(), 0));
		freeCam.lookAt(tank1.getPosition() + glm::vec3(0, 3, 0), { 0,1,0 });
		//freeCam.setAxes(glm::quat({ roll, yaw, 0 }));
        freeCam.update();
		
		tank1.update((float)currentTime);
		tank2.update((float)currentTime);
		tankwars::explosionHandler->update(frameTime);

        // Render
        int backBufferWidth, backBufferHeight;
        glfwGetFramebufferSize(window, &backBufferWidth, &backBufferHeight);
        renderer.setBackBufferSize(backBufferWidth, backBufferHeight);
        renderer.render();

        glfwSwapBuffers(window);

        // Update events and input
        tankwars::Keyboard::update();
        glfwPollEvents();
    }

    // Clean up
    glDeleteTextures(1, &hudSprite.texture);
    glDeleteTextures(1, &hudSprite2.texture);
    glDeleteTextures(1, &hudSprite3.texture);
    glDeleteTextures(1, &hudSprite32.texture);
    glDeleteTextures(1, &hudSprite4tr.texture);
    glDeleteTextures(1, &hudSprite4.texture);
    glDeleteTextures(1, &hudSprite42.texture);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
