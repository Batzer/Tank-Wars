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

	//TESTINGQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
	/*btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* groundMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 15, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);
	*/
    // Init input systems
    tankwars::Keyboard::init();
    glfwSetKeyCallback(window, &tankwars::Keyboard::keyCallback);
    glfwSetWindowFocusCallback(window, &tankwars::Keyboard::windowFocusCallback);

    // The game loop
    auto lastTime = glfwGetTime();

    // TEST
    tankwars::Renderer renderer;
    tankwars::VoxelTerrain terrain2 = tankwars::VoxelTerrain::fromHeightMap("Content/Maps/test_very_big.png",
        dynamicsWorld.get(), 16, 8, 16, 8);
    renderer.setTerrain(&terrain2);
	
    tankwars::Tank tank1(dynamicsWorld.get(),renderer, btVector3(30, 25, -30),0);
	tankwars::Tank tank2(dynamicsWorld.get(), renderer, btVector3(40, 25, -40), 1);
	gContactAddedCallback = tankwars::customCallback;


    /*btCollisionShape* fallShape = new btSphereShape(2);
    btDefaultMotionState* fallMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(20, 50, -20)));
    btScalar mass = 50;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);*/
    //dynamicsWorld->addRigidBody(fallRigidBody);

	
    tankwars::Mesh sphereMesh = tankwars::createSphereMesh(2, 16, 16);
    tankwars::Material mat;
    mat.specularExponent = 256;
    tankwars::MeshInstance sphere(sphereMesh, mat);
    renderer.addSceneObject(sphere);

    /*auto smokeTexture = tankwars::createTextureFromFile("Content/Textures/comic-boom-explosion.png");
    tankwars::ParticleSystem particleSystem(512, smokeTexture);
    particleSystem.setEmitterPosition({30, 30, -30});
    particleSystem.setParticleColorRange({1, 1, 1, 0.25f}, {1, 1, 1, 0.75f});
    particleSystem.setParticleSizeRange(1, 2);
    particleSystem.setParticleLifeTimeRange(3, 4);
    renderer.addParticleSystem(particleSystem);*/
    
    float roll = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float camSpeed = 20.0f;
    tankwars::Camera freeCam;
    freeCam.aspectRatio = 16 / 4.5f;
    freeCam.position = { 10, 40, 10 };
    renderer.attachCamera(tankwars::Renderer::ViewportTop, freeCam);
	tankwars::Game game(&freeCam, &terrain2);
	tankwars::explosionHandler = new tankwars::ExplosionHandler(dynamicsWorld.get(), renderer, terrain2, &tank1, &tank2, &game);
    
    tankwars::Camera freeCam2;
    freeCam2.position = { 15, 40, 10 };
    freeCam2.aspectRatio = 16 / 4.5f;
    renderer.attachCamera(tankwars::Renderer::ViewportBottom, freeCam2);
    renderer.setSplitScreenEnabled(true);

	game.setupControllers();
	game.bindControllerToTank(0, &tank1);
	game.bindControllerToTank(1, &tank2);
    
    tankwars::HudSprite hudSprite;
	hudSprite.position = { -0.91 , -0.5};
	hudSprite.transparency = 1;
	hudSprite.size = { 0.3, 1.8 };
    hudSprite.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy.png");

	tankwars::HudSprite hudSprite2;
	hudSprite2.position = { -0.91 , -0.5 };
	hudSprite2.transparency = 1;
	hudSprite2.size = { 0.3, 1.8 };
	hudSprite2.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy2.png");

	tankwars::HudSprite hudSprite3;
	hudSprite3.position = { -0.91 , -0.5 };
	hudSprite3.transparency = 1;
	hudSprite3.size = { 0.3, 1.8 };
	hudSprite3.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy3.png");

	tankwars::HudSprite hudSprite4tr;
	hudSprite4tr.position = { -0.91 , -0.5 };
	hudSprite4tr.transparency = 0.5;
	hudSprite4tr.size = { 0.3, 1.8 };
	hudSprite4tr.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy4.png");

	tankwars::HudSprite hudSprite4;
	hudSprite4.position = { -0.91 , -0.5 };
	hudSprite4.transparency = 1;
	hudSprite4.size = { 0.3, 1.8 };
	hudSprite4.texture = tankwars::createTextureFromFile("Content/Hud/gamethingy4.png");

    tankwars::Hud hud1;
    hud1.addSprite(hudSprite, 4);
	hud1.addSprite(hudSprite2, 3);
	hud1.addSprite(hudSprite3, 0);
	hud1.addSprite(hudSprite4tr, 1);
	hud1.addSprite(hudSprite4, 2);
    renderer.attachHud(tankwars::Renderer::ViewportTop, hud1);


    tankwars::Hud hud2;
    hud2.addSprite(hudSprite, 0);
    renderer.attachHud(tankwars::Renderer::ViewportBottom, hud2);

    while (!glfwWindowShouldClose(window)) {
        auto currentTime = glfwGetTime();
        auto frameTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        // Update simulation
        dynamicsWorld->stepSimulation(frameTime, 15, 1.0f / 120.0f);

        // Update game here
		game.update((float)currentTime);
		if (tankwars::Keyboard::isKeyDown(GLFW_KEY_T)) {
			hudSprite4.texSize[1] -= 0.01;
			hudSprite4.size[1] -= 0.03;
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
		if (tankwars::Keyboard::isKeyDown(GLFW_KEY_P))//particleSystem.emit(1);
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

        //terrain2.updateMesh();

		tankwars::explosionHandler->update(frameTime);
        // TEST
        
        /*btTransform tr;
        fallRigidBody->getMotionState()->getWorldTransform(tr);
        tr.getOpenGLMatrix(glm::value_ptr(sphere.modelMatrix));*/
        
        // END

        /*particleSystem.update(frameTime, [&](tankwars::Particle& p) {
            p.color.a -= 0.3f * frameTime;
            if (p.color.a < 0.0f) {
                p.isAlive = false;
            }
        });*/

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
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}