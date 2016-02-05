#include "ExplosionHandling.h"

namespace tankwars {
    ExplosionHandler* explosionHandler;
    
    void ExplosionHandler::update() {
		handleExplosions();
		//do sth...								update explosion Animations
	}
	void ExplosionHandler::addExplosionPoint(btVector3 explosionAt) {
		explosionPoints.push_back(explosionAt);
	}
	void ExplosionHandler::explosion(btVector3 expl) {					// recalculate boundaries
		//do sth...								start explosion Animations
		int xMin = std::max((int)(expl.getX() - explRadius), 1);
		int yMin = std::max((int)(expl.getY() - explRadius), 1);
		int zMin = std::max((int)(expl.getZ() + explRadius), -(int)terrain.getDepth()+1);
		int xMax = std::min((int)(expl.getX() + explRadius + 0.5), (int)terrain.getWidth()-1);
		int yMax = std::min((int)(expl.getY() + explRadius + 0.5), (int)terrain.getHeight()-1);
		int zMax = std::min((int)(expl.getZ() - explRadius + 0.5), 1);
		for (int x = xMin; x < xMax; x++) {
			for (int y = yMin; y < yMax; y++) {
				for (int z = zMin; z > zMax; z--) {
					if (pow(x - expl.getX(), 2) + std::pow(y - expl.getY(), 2) + pow(z - expl.getZ(), 2) < pow(explRadius,2))
						terrain.setVoxel(x, y, -z, tankwars::VoxelType::Empty);
				}
			}
		}
        terrain.updateMesh();
	}
	void ExplosionHandler::handleExplosions() {
		while (explosionPoints.size()) {
			explosion(explosionPoints.back());
			explosionPoints.pop_back();
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
			if (bulletCollision == 1 || bulletCollision == 3) {
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