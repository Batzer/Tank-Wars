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
#include "ExplosionHandling.h"

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

	//tankwars::Terrain terrain("Content/Maps/Penis.bmp", 2);
	
    tankwars::Tank tank1(dynamicsWorld.get(),renderer, btVector3(30, 25, -30));
	gContactAddedCallback = tankwars::customCallback;
	tankwars::explosionHandler = new tankwars::ExplosionHandler(dynamicsWorld.get(), renderer, terrain2);
    
	//dynamicsWorld->addAction(tank1.getAction());
	//dynamicsWorld->addRigidBody(tank1.getRigidBody());

   /* auto tankBodyModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankBody.obj");
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
    tankCanonInstance.modelMatrix = tankModelMat;*/

    /*renderer.addSceneObject(tankBodyInstance);
    renderer.addSceneObject(tankHeadInstance);
    renderer.addSceneObject(tankCanonInstance);*/
    
    btCollisionShape* fallShape = new btSphereShape(2);
    btDefaultMotionState* fallMotionState =
        new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(20, 50, -20)));
    btScalar mass = 50;
    btVector3 fallInertia(0, 0, 0);
    fallShape->calculateLocalInertia(mass, fallInertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
    btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
    //dynamicsWorld->addRigidBody(fallRigidBody);

	
    tankwars::Mesh sphereMesh = tankwars::createSphereMesh(2, 16, 16);
    tankwars::Material mat;
    mat.specularExponent = 256;
    tankwars::MeshInstance sphere(sphereMesh, mat);
    renderer.addSceneObject(sphere);
    
    float roll = 0.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    float camSpeed = 20.0f;
    tankwars::Camera freeCam;
    freeCam.position = { 10, 40, 10 };
	tankwars::Game game(&freeCam);
    
	game.setupControllers();
	game.bindControllerToTank(0, &tank1);
	game.bindControllerToTank(1, &tank1);
    
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = glfwGetTime();
        auto frameTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        // Update simulation
		
        dynamicsWorld->stepSimulation(frameTime, 7);

        // Update game here
		game.update((float)currentTime);
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
		//if (tankwars::Keyboard::isKeyDown(GLFW_KEY_P))dynamicsWorld->removeRigidBody(groundRigidBody);
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
		freeCam.position = tank1.getPosition()+glm::normalize(-tank1.getDirectionVector())*15.f+glm::vec3(0,6,0);
		freeCam.setAxes(glm::quat({ roll, yaw, 0 }));
		freeCam.lookAt(tank1.getPosition(), { 0,1,0 });
        freeCam.update();
		
		tank1.update();

        //terrain2.updateMesh();

		tankwars::explosionHandler->update();
        // TEST
        
        btTransform tr;
        fallRigidBody->getMotionState()->getWorldTransform(tr);
        tr.getOpenGLMatrix(glm::value_ptr(sphere.modelMatrix));
        
        // END

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