#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "Terrain.h"
#include "Camera.h"

namespace tankwars {
	class Game {
	public:
		Game(Camera * camera);
		~Game();
		int setupControllers();
		void addCamera(Camera * camera);
		void addTerrain(Terrain * terrain);
		void update(float dt);
	private:
		void pew();
		void controller();
		Camera * camera;
		Terrain * terrain;
		int joystickAvailable[2];
		float explosion_radius;
	};
}