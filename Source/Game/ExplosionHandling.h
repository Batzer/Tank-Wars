#pragma once
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <algorithm>
#include <vector>
#include "Renderer.h"
#include "Tank.h"
#include "Game.h"

namespace tankwars {
	bool customCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);

	class ExplosionHandler {
	public:
		ExplosionHandler(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, VoxelTerrain& terrain, Tank* tank1, Tank* tank2, Game* game) :game(game),dnmcWrld(dynamicsWorld), renderer(renderer), terrain(terrain) { tanks[0] = tank1; tanks[1] = tank2;};
		void addExplosionPoint(btVector3 explosionAt, int owner);
		void update();
	private:
		Game* game;
		btScalar tankRadius = 1.5f;				//adjust--------------------------------------------------------------------------
		Tank* tanks[2];
		btScalar explRadius = 2.5f;
		void handleExplosions();
		void explosion(std::pair<btVector3, int> pair);
		std::vector<std::pair<btVector3,int>> explosionPoints;
		btDiscreteDynamicsWorld* dnmcWrld;
		Renderer& renderer;
		VoxelTerrain& terrain;
	};
	extern ExplosionHandler* explosionHandler;
}