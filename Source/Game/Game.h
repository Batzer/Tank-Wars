#pragma once
#include <iostream>
#include "Terrain.h"
#include <GLFW/glfw3.h>
#include "Tank.h"
#include "Camera.h"

namespace tankwars {
	class Game {
	public:
		Game(Camera * camera);

		int setupControllers();
		void addCamera(Camera * camera);
		void addTerrain(Terrain * terrain);
		void update(float dt);
		void render();
		void bindControllerToTank(int controllerID, Tank* tank);

	private:
		float timeBetweenShots = 0.5f;
		float lastShot = 0;
		Tank* tanks[2];
		void pew();
		void controller(float dt);

		Camera* camera;
		Terrain* terrain;
		int joystickAvailable[2];
		float explosion_radius = 3;
	};
}
