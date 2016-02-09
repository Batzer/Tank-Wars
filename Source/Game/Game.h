#pragma once

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <glm/glm.hpp>

namespace tankwars {
    class Camera;
    class Tank;
    class VoxelTerrain;

	class Game {
	public:
		Game(Camera* camera, VoxelTerrain* ter);

		int setupControllers();
		void addCamera(Camera* camera);
		void update(float dt);
		void render();
		void bindControllerToTank(int controllerID, Tank* tank);
		void tankGotHit(int index);
		void reset();

	private:
		btScalar getBestHeightFor(btVector3 pos);
		btScalar getBestHeightFor2(btVector3 pos);
		bool isPlaneClear(btVector3 vec, int height);
        void controller(float dt);
		bool closer(btVector3 vec1, btVector3 vec2, glm::vec3 distanceTo);

		btScalar tankRadius = 1.5f;
		btScalar spawnOffset = 12;
		btVector3 spawnCoordinates[4];
		Tank* tanks[2];
		Camera* camera;
		VoxelTerrain* terrain;
		int joystickAvailable[2];
		float explosion_radius = 3;
		btScalar lastPositionChange = .0f;
		btScalar timeBetweenPositionChanges = 3.f;
	};
}
