#include "Game.h"
namespace tankwars {
	Game::Game(Camera * camera)
            : camera(camera) {
        // Do nothing
    }

	int Game::setupControllers() {
        // NOTE: Maybe map the functions to the keys, so that they are not hard-coded?
		joystickAvailable[0] = glfwJoystickPresent(GLFW_JOYSTICK_1);
		joystickAvailable[1] = glfwJoystickPresent(GLFW_JOYSTICK_2);
		std::cout << "Available joysticks: 1st-" << joystickAvailable[0] << " 2nd-" << joystickAvailable[1];
		return joystickAvailable[0] + joystickAvailable[1];
	}

	void Game::addCamera(Camera* camera) {
		this->camera = camera;
	}

	void Game::addTerrain(Terrain* terrain) {
		this->terrain = terrain;
	}
	void Game::bindControllerToTank(int controllerID, Tank* tank) {
		tanks[controllerID] = tank;
	}
	void Game::update(float dt) {
		(void)dt;
		controller();
	}

	void Game::render() {

	}

	void Game::controller() {
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
					tanks[1]->driveController(axis[i]);
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
						std::cout << "|>" << "\n";
					}
					break;
				case 1:
					if (axes[i]) {
						std::cout << "O" << "\n";
					}
					break;
				case 2:
					if (axes[i]) {
						//std::cout << "X" << "\n";
						tanks[1]->shoot();
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
						std::cout << "L1" << "\n";
                        //camera->rotate(glm::quat({rotation_alpha, 0, 0}));
					}
					break;
				case 5:
					if (axes[i]) {
						std::cout << "R1" << "\n";
                        //camera->rotate(glm::quat({-rotation_alpha, 0, 0}));
					}
					break;
				case 6:
					if (axes[i]) {
						//std::cout << "L2" << "\n";
						if (explosion_radius > 0) {
							explosion_radius -= 0.1f;
							std::cout << "Explosion radius changed to: " << explosion_radius << "\n";
						}
					}
					break;
				case 7:
					if (axes[i]) {
						//std::cout << "R2" << "\n";
						if (explosion_radius < 9.9) {
							explosion_radius += 0.1f;
							std::cout << "Explosion radius changed to: " << explosion_radius << "\n";
						}
					}
					break;

				case 8:
					if (axes[i]) {
						std::cout << "Select" << "\n";
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

	void Game::pew() {
        const auto& camPos = camera->position;
        glm::vec3 location(camPos.x, terrain->getHeightAt(camPos.x, camPos.z), camPos.z);
		terrain->explosionAt(location, explosion_radius);
	}
}