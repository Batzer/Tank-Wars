#include "Game.h"
namespace tankwars {
	Game::Game(Camera * camera, VoxelTerrain* ter)
            : camera(camera),terrain(ter) {
		spawnCoordinates[0] = btVector3(spawnOffset,0, spawnOffset);
		spawnCoordinates[1] = btVector3(spawnOffset,0,ter->getDepth()- spawnOffset);
		spawnCoordinates[2] = btVector3(ter->getWidth()- spawnOffset,0, ter->getDepth() - spawnOffset);
		spawnCoordinates[3] = btVector3(ter->getWidth() - spawnOffset,0,spawnOffset);
    }

	int Game::setupControllers() {
        // NOTE: Maybe map the functions to the keys, so that they are not hard-coded?
		joystickAvailable[0] = glfwJoystickPresent(GLFW_JOYSTICK_1);
		joystickAvailable[1] = glfwJoystickPresent(GLFW_JOYSTICK_2);
		std::cout << "Available joysticks: 1st-" << joystickAvailable[0] << " 2nd-" << joystickAvailable[1];
		return joystickAvailable[0] + joystickAvailable[1];
	}
	void Game::tankGotHit(int index) {
		tanks[1 ^ index]->addPoint();
		int closestPointToEnemy = 0;
		glm::vec3 posi(tanks[1 ^ index]->getPosition().x, tanks[1 ^ index]->getPosition().y, -tanks[1 ^ index]->getPosition().z);
		for (int i = 0; i < 3; i++) {
			if (!closer(spawnCoordinates[i], spawnCoordinates[i + 1],posi)) {
				closestPointToEnemy = i + 1;
			}
		}
		std::random_device r;
		std::default_random_engine e1(r());
		std::uniform_int_distribution<int> uniform_dist(0, 2);
		int mean = uniform_dist(e1);
		if (mean >= closestPointToEnemy) {
			mean++;
		}
		int height = getBestHeightFor(spawnCoordinates[mean]);
		/*for (height = terrain->getHeight()-3; height > 3; height--) {
			if (!isPlaneClear(spawnCoordinates[mean],height)) {
				break;
			}
		}
		height+=4;*/
		tanks[index]->reset(glm::vec3(spawnCoordinates[mean].getX(), height, -spawnCoordinates[mean].getZ()), glm::vec3(terrain->getWidth()/2,0,terrain->getDepth()));
	}
	btScalar Game::getBestHeightFor(btVector3 pos) {
		int height;
		for (height = terrain->getHeight() - 3; height > 3; height--) {
			if (!isPlaneClear(pos, height)) {
				break;
			}
		}
		return height + 4;
	}
	bool Game::isPlaneClear(btVector3 pos, int height) {
		for (int x = pos.getX() - tankRadius; x < pos.getX() + tankRadius; x++) {
			for (int z = pos.getZ() - tankRadius; z < pos.getZ() + tankRadius; z++) {
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
		//name
		//const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
		if (joystickAvailable[1]) {
			//axis
			int count;
			const float* axis = glfwGetJoystickAxes(GLFW_JOYSTICK_2, &count);
			for (char i = 0; i < count; i++) {
				switch (i) {
				case 0:		//L-stick x-Axis
					tanks[1]->turnController(axis[i]);
					break;
				case 1:		//L-stick y-Axis
					
					break;
				case 2:		//R-stick x-Axis
					tanks[1]->turnHeadAndTurretController(axis[i]);
					break;
				case 3:		//R-stick y-Axis
					tanks[1]->turnTurretController(axis[i]);
					break;
				}
			}
			//std::cout << axis[0] << "\t" << axis[1] << "\t" << axis[2] << "\t" << axis[3]<<"\n";
			//buttons
			count;
			const unsigned char* axes = glfwGetJoystickButtons(GLFW_JOYSTICK_2, &count);
			for (char i = 0; i < count; i++) {
				switch (i) {
				case 0:
					if (axes[i]) {
						//std::cout << "|>" << "\n";
						if (dt - lastPositionChange > timeBetweenPositionChanges) {
							tankGotHit(0);
							lastPositionChange = dt;
						}
					}
					break;
				case 1:
					if (axes[i]) {
						std::cout << "O" << "\n";
						glm::vec3 pos = tanks[0]->getPosition();
						pos.y = getBestHeightFor(btVector3(pos.x,0, -pos.z));
						tanks[0]->reset(pos, -tanks[0]->getDirectionVector());
					}
					break;
				case 2:
					if (axes[i]) {
						//std::cout << "X" << "\n";
						tanks[1]->shoot(dt);
					}
					break;
				case 3:
					if (axes[i]) {
						//std::cout << "|_|" << "\n";
						tanks[1]->breakController();
					}
					break;
				case 4:
					if (axes[i]) {
						//std::cout << "L1" << "\n";
						tanks[1]->driveController(false);
					}
					break;
				case 5:
					if (axes[i]) {
						//std::cout << "R1" << "\n";
						tanks[1]->driveController(true);
					}
					break;
				case 6:
					if (axes[i]) {
						//std::cout << "L2" << "\n";
						tanks[1]->adjustPower(false, dt);
					}
					break;
				case 7:
					if (axes[i]) {
						//std::cout << "R2" << "\n";
						tanks[1]->adjustPower(true, dt);
					}
					break;

				case 8:
					if (axes[i]) {
						std::cout << "Select" << "\n";
						//reset tank toi higher position
					}
					break;
				case 9:
					if (axes[i]) {
						std::cout << "Start" << "\n";
					}
					break;
				case 10:
					if (axes[i]) {
						std::cout << "L3" << "\n";
					}
					break;
				case 11:
					if (axes[i]) {
						std::cout << "R3" << "\n";
					}
					break;

				case 12:
					if (axes[i]) {
						std::cout << "pad_up" << "\n";
                        camera->position += glm::vec3(0, 0, 1) * movement_alpha;
					}
					break;
				case 13:
					if (axes[i]) {
						std::cout << "pad_right" << "\n";
                        camera->position += glm::vec3(1, 0, 0) * movement_alpha;
					}
					break;
				case 14:
					if (axes[i]) {
						std::cout << "pad_down" << "\n";
                        camera->position -= glm::vec3(0, 0, 1) * movement_alpha;
					}
					break;
				case 15:
					if (axes[i]) {
						std::cout << "pad_left" << "\n";
                        camera->position -= glm::vec3(1, 0, 0) * movement_alpha;
					}
					break;
				default:
					break;

				}
			}
		}
		if (joystickAvailable[0]) {

		}
	}
}