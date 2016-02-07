#pragma once


#include <algorithm>
#include <vector>
#include <memory>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "Renderer.h"
#include "Tank.h"
#include "Game.h"
#include "ParticleSystem.h"
#include "GLTools.h"

namespace tankwars {
	bool customCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);

	class ExplosionHandler {
	public:
		ExplosionHandler(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, VoxelTerrain& terrain, Tank* tank1, Tank* tank2, Game* game);
        ~ExplosionHandler();
		void addExplosionPoint(btVector3 explosionAt, int owner);
		void update(btScalar dt);

	private:
        void handleExplosions();
		void explosion(std::pair<btVector3, int> pair);

		GLuint smokeTexture;
		ParticleSystem smokeParticleSystem;
		Game* game;
		btScalar tankRadius = 1.5f;				//adjust--------------------------------------------------------------------------
		Tank* tanks[2];
		btScalar explRadius = 3.5f;
		std::vector<std::pair<btVector3,int>> explosionPoints;
		btDiscreteDynamicsWorld* dnmcWrld;
		Renderer& renderer;
		VoxelTerrain& terrain;
	};

	extern std::unique_ptr<ExplosionHandler> explosionHandler;
}