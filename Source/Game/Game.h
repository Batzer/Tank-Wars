#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "Terrain.h"
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

	private:
		void pew();
		void controller();

		Camera* camera;
		Terrain* terrain;
		int joystickAvailable[2];
		float explosion_radius = 3;
	};
}