#pragma once
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <vector>
#include "Renderer.h"
#include "Tank.h"
namespace tankwars {
	bool customCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);

	class ExplosionHandler {
	public:
		ExplosionHandler(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer) :dnmcWrld(dynamicsWorld), renderer(renderer) {};
		void addExplosionPoint(btVector3 explosionAt);

		void update();
	private:
		void handleExplosions();
		void explosion(btVector3 expl);
		std::vector<btVector3> explosionPoints;
		btDiscreteDynamicsWorld* dnmcWrld;
		Renderer& renderer;
	};
	ExplosionHandler* explosionHandler;
}