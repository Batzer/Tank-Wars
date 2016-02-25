#include "Game.h"

#include <cmath>
#include <iostream>
#include <cstring>

#include <GL/gl3w.h> // To be sure
#include <GLFW/glfw3.h>

#include "VoxelTerrain.h"
#include "Tank.h"
#include "Camera.h"

namespace {
    // Super ugly, but define a preset for the XBox 360 Controller
    // and another one that seems to work with most controllers that
    // have a Playstation layout
    constexpr int GenericTurn = 0;
    constexpr int GenericRotateHead = 2;
    constexpr int GenericRotateTurret = 3;
    constexpr int GenericToggleShootingMode = 0;
    constexpr int GenericShoot = 2;
    constexpr int GenericBreak = 3;
    constexpr int GenericDriveForward = 5;
    constexpr int GenericDriveBackward = 4;
    constexpr int GenericDecrPower = 6;
    constexpr int GenericIncrPower = 7;
    constexpr int GenericReset = 8;
    constexpr int GenericZoomOut = 12;
    constexpr int GenericZoomIn = 14;

    constexpr int XBoxTurn = 0;
    constexpr int XBoxRotateHead = 4;
    constexpr int XBoxRotateTurret = 3;
    constexpr int XBoxToggleShootingMode = 0;
    constexpr int XBoxShoot = 2;
    constexpr int XBoxBreak = 3;
    constexpr int XBoxDriveForward = 5;
    constexpr int XBoxDriveBackward = 4;
    constexpr int XBoxDecrPower = 6;
    constexpr int XBoxIncrPower = 7;
    constexpr int XBoxReset = 8;
    constexpr int XBoxZoomOut = 10;
    constexpr int XBoxZoomIn = 12;

    const char* XboxControllerName = "Microsoft PC-joystick driver";
}

namespace tankwars {
	Game::Game(Camera * camera, VoxelTerrain* ter)
            : camera(camera),terrain(ter),
              randomEngine(randomDevice()),
              uniformDist(0, 3) {
		spawnCoordinates[0] = btVector3(spawnOffset, 0, spawnOffset);
		spawnCoordinates[1] = btVector3(spawnOffset, 0, ter->getDepth() - spawnOffset);
		spawnCoordinates[2] = btVector3(ter->getWidth() - spawnOffset, 0, ter->getDepth() - spawnOffset);
		spawnCoordinates[3] = btVector3(ter->getWidth() - spawnOffset, 0, spawnOffset);
    }

	int Game::setupControllers(bool disableXboxHack) {
        // NOTE: Maybe map the functions to the keys, so that they are not hard-coded?
        for (int i = 0; i < 2; i++) {
            joystickAvailable[i] = glfwJoystickPresent(GLFW_JOYSTICK_1 + i);

            if (joystickAvailable[i]) {
                auto name = glfwGetJoystickName(GLFW_JOYSTICK_1 + i);
                isXboxController[i] = (strcmp(name, XboxControllerName) == 0);
                if (!disableXboxHack) isXboxController[i] = (i == 0); // HACK: Glfw sees no difference, so still hardcode controller 1 for xbox
                std::cout << "Joystick " << (i + 1) << ": \"" << name << "\"\n";

                joystickConfigs[i].Turn               = isXboxController[i] ? XBoxTurn : GenericTurn;
                joystickConfigs[i].RotateHead         = isXboxController[i] ? XBoxRotateHead : GenericRotateHead;
                joystickConfigs[i].RotateTurret       = isXboxController[i] ? XBoxRotateTurret : GenericRotateTurret;
                joystickConfigs[i].ToggleShootingMode = isXboxController[i] ? XBoxToggleShootingMode : GenericToggleShootingMode;
                joystickConfigs[i].Shoot              = isXboxController[i] ? XBoxShoot : GenericShoot;
                joystickConfigs[i].Break              = isXboxController[i] ? XBoxBreak : GenericBreak;
                joystickConfigs[i].DriveForward       = isXboxController[i] ? XBoxDriveForward : GenericDriveForward;
                joystickConfigs[i].DriveBackward      = isXboxController[i] ? XBoxDriveBackward : GenericDriveBackward;
                joystickConfigs[i].DecrPower          = isXboxController[i] ? XBoxDecrPower : GenericDecrPower;
                joystickConfigs[i].IncrPower          = isXboxController[i] ? XBoxIncrPower : GenericIncrPower;
                joystickConfigs[i].Reset              = isXboxController[i] ? XBoxReset : GenericReset;
                joystickConfigs[i].ZoomOut            = isXboxController[i] ? XBoxZoomOut : GenericZoomOut;
                joystickConfigs[i].ZoomIn             = isXboxController[i] ? XBoxZoomIn : GenericZoomIn;
            }
        }

		return joystickAvailable[0] + joystickAvailable[1];
	}
	void Game::reset() {
		int height = static_cast<int>(getBestHeightFor(spawnCoordinates[0]));
		tanks[0]->reset(glm::vec3(spawnCoordinates[0].getX(), height, -spawnCoordinates[0].getZ()), glm::vec3(terrain->getWidth() / 2, 0, terrain->getDepth()));

		height = static_cast<int>(getBestHeightFor(spawnCoordinates[2]));
		tanks[1]->reset(glm::vec3(spawnCoordinates[2].getX(), height, -spawnCoordinates[2].getZ()), glm::vec3(terrain->getWidth() / 2, 0, terrain->getDepth()));
	}
	void Game::tankGotHit(int index) {
        auto tankIndex = 1 ^ index;
		tanks[tankIndex]->addPoint();

		int closestPointToEnemy = 0;
        const auto& tankPos = tanks[tankIndex]->getPosition();
		glm::vec3 pos(tankPos.x, tankPos.y, -tankPos.z);

		for (int i = 1; i < 4; i++) {
			if (closer(spawnCoordinates[i], spawnCoordinates[closestPointToEnemy], pos)) {
				closestPointToEnemy = i;
			}
		}

        int spawnIndex = uniformDist(randomEngine);
        while (spawnIndex == closestPointToEnemy) {
            spawnIndex = uniformDist(randomEngine);
        }

        const auto& spawnLocation = spawnCoordinates[spawnIndex];
		int height = static_cast<int>(getBestHeightFor(spawnLocation));
        glm::vec3 spawnPos(spawnLocation.getX(), height, -spawnLocation.getZ());
        glm::vec3 lookAt(terrain->getWidth() / 2, height, -static_cast<float>(terrain->getDepth() / 2));
		tanks[index]->reset(spawnPos, lookAt);
	}
	btScalar Game::getBestHeightFor(btVector3 pos) {
		int height;
		for (height = static_cast<int>(terrain->getHeight() - 3); height > 3; height--) {
			if (!isPlaneClear(pos, height)) {
				break;
			}
		}
		return static_cast<btScalar>(height + 3);
	}
	btScalar Game::getBestHeightFor2(btVector3 pos) {
		int height;
		bool NotClear = true;
		for (height = static_cast<int>(pos.getY() + 5); height > 3; height--) {
			if (NotClear && isPlaneClear(pos, height)) {
				NotClear = false;
			}
			else if(!NotClear && !isPlaneClear(pos, height)) {
				break;
			}
		}
		return static_cast<btScalar>(height + 2);
	}
	bool Game::isPlaneClear(btVector3 pos, int height) {
		for (int x = static_cast<int>(pos.getX() - tankRadius); x < pos.getX() + tankRadius; x++) {
			for (int z = static_cast<int>(pos.getZ() - tankRadius); z < pos.getZ() + tankRadius; z++) {
				if (pow(x - pos.getX(), 2) + pow(z - pos.getZ(), 2) < pow(tankRadius, 2)) {
					if (terrain->getVoxel(x, height, z) == VoxelType::Solid) {
						return false;
					}
				}
			}	
		}
		return true;
	}
	bool Game::closer(btVector3 vec1, btVector3 vec2, glm::vec3 distanceTo) {
		btScalar distVec1 = pow(vec1.getX() - distanceTo.x, 2)+ pow(vec1.getY() - distanceTo.y, 2)+ pow(vec1.getZ() - distanceTo.z, 2);
		btScalar distVec2 = pow(vec2.getX() - distanceTo.x, 2) + pow(vec2.getY() - distanceTo.y, 2) + pow(vec2.getZ() - distanceTo.z, 2);
		if (distVec1 < distVec2)
			return true;
		return false;
	}
	void Game::addCamera(Camera* camera) {
		this->camera = camera;
	}
	void Game::bindControllerToTank(int controllerID, Tank* tank) {
		tanks[controllerID] = tank;
	}
	void Game::update(float dt) {
		controller(dt);
	}

	void Game::render() {

	}

	void Game::controller(float dt) {
		float movement_alpha = 1;
		float rotation_alpha = 0.1f;

        for (int i = 0; i < 2; i++) {
            if (!joystickAvailable[i]) {
                continue;
            }

            int count;
            auto axis = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &count);
            auto buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + i, &count);

            auto value = axis[joystickConfigs[i].Turn];
            if (abs(value) > 0.2f) {
                tanks[i]->turnController(value);
            }
            else {
                tanks[i]->turnController(0.0f);
            }

            value = axis[joystickConfigs[i].RotateHead];
            if (abs(value) > 0.2f) {
                tanks[i]->turnHeadAndTurretController(value);
            }

            value = axis[joystickConfigs[i].RotateTurret];
            if (abs(value) > 0.2f) {
                tanks[i]->turnTurretController(value);
            }

            if (buttons[joystickConfigs[i].ToggleShootingMode]) tanks[i]->toggleShootingMode(dt);
            if (buttons[joystickConfigs[i].Shoot])              tanks[i]->shoot(dt);
            if (buttons[joystickConfigs[i].Break])              tanks[i]->breakController();
            if (buttons[joystickConfigs[i].DriveBackward])      tanks[i]->driveController(false);
            if (buttons[joystickConfigs[i].DriveForward])       tanks[i]->driveController(true);
            if (buttons[joystickConfigs[i].DecrPower])          tanks[i]->adjustPower(false, dt);
            if (buttons[joystickConfigs[i].IncrPower])          tanks[i]->adjustPower(true, dt);
            if (buttons[joystickConfigs[i].ZoomOut])            tanks[i]->moveCam(false, dt);
            if (buttons[joystickConfigs[i].ZoomIn])             tanks[i]->moveCam(true, dt);
            if (buttons[joystickConfigs[i].Reset]) {
                glm::vec3 pos = tanks[i]->getPosition();
                pos.y = getBestHeightFor2(btVector3(pos.x, pos.y, -pos.z));
                tanks[i]->reset(pos, -tanks[i]->getDirectionVector());
            }      
        }
	}
}
