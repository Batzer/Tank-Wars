#include "ExplosionHandling.h"

namespace tankwars {
	void ExplosionHandler::update() {
		handleExplosions();
		//do sth...								update explosion Animations
	}
	void ExplosionHandler::addExplosionPoint(btVector3 explosionAt) {
		explosionPoints.push_back(explosionAt);
	}
	void ExplosionHandler::explosion(btVector3 expl) {
		//do sth...								start explosion Animations
	}
	void ExplosionHandler::handleExplosions() {
		while (explosionPoints.size()) {
			explosion(explosionPoints.back());
		}
	}
	bool customCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
		int bulletCollision = 0;
		if (colObj0Wrap->getCollisionObject()->getUserIndex() == 10) {
			Tank::Bullet* bullet = ((Tank::Bullet*)colObj0Wrap->getCollisionObject()->getUserPointer());
			bullet->disableMe = true;
			bulletCollision = 1;
		}
		if (colObj1Wrap->getCollisionObject()->getUserIndex() == 10) {
			Tank::Bullet* bullet = ((Tank::Bullet*)colObj1Wrap->getCollisionObject()->getUserPointer());
			bullet->disableMe = true;
			bulletCollision += 2;
		}
		if (bulletCollision) {
			if (bulletCollision == 1) {
				explosionHandler->addExplosionPoint(cp.getPositionWorldOnA());
				bulletCollision -= 1;
			}
			if (bulletCollision == 2) {
				explosionHandler->addExplosionPoint(cp.getPositionWorldOnB());
			}
		}
		return false;
	}
}