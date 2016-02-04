#pragma once
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <algorithm>
#include <vector>
#include "Renderer.h"
#include "Tank.h"

namespace tankwars {
	bool customCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);

	class ExplosionHandler {
	public:
		ExplosionHandler(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, VoxelTerrain& terrain) :dnmcWrld(dynamicsWorld), renderer(renderer), terrain(terrain) {};
		void addExplosionPoint(btVector3 explosionAt);

		void update();
	private:
		btScalar explRadius = 2.5f;
		void handleExplosions();
		void explosion(btVector3 expl);
		std::vector<btVector3> explosionPoints;
		btDiscreteDynamicsWorld* dnmcWrld;
		Renderer& renderer;
		VoxelTerrain& terrain;
	};
	extern ExplosionHandler* explosionHandler;
}