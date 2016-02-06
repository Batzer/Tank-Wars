#include "ExplosionHandling.h"

namespace tankwars {
    ExplosionHandler* explosionHandler;
    
    void ExplosionHandler::update() {
		handleExplosions();
		//do sth...								update explosion Animations
	}
	void ExplosionHandler::addExplosionPoint(btVector3 explosionAt,int owner) {
		explosionPoints.push_back(std::make_pair(explosionAt,owner));
	}
	void ExplosionHandler::explosion(std::pair<btVector3,int> pair) {
		btVector3 expl(pair.first.getX(), pair.first.getY(), -pair.first.getZ());
		int xMin = std::max((int)(expl.getX() - explRadius), 1);
		int yMin = std::max((int)(expl.getY() - explRadius), 1);
		int zMin = std::max((int)(expl.getZ() - explRadius), 1);
		int xMax = std::min((int)(expl.getX() + explRadius + 0.5), (int)terrain.getWidth()-1);
		int yMax = std::min((int)(expl.getY() + explRadius + 0.5), (int)terrain.getHeight()-1);
		int zMax = std::min((int)(expl.getZ() + explRadius + 0.5), (int)terrain.getDepth()-1);
		for (int x = xMin; x < xMax; x++) {
			for (int y = yMin; y < yMax; y++) {
				for (int z = zMin; z < zMax; z++) {
					if (pow(x - expl.getX(), 2) + pow(y - expl.getY(), 2) + pow(z - expl.getZ(), 2) < pow(explRadius, 2)) {
						terrain.setVoxel(x, y, z, tankwars::VoxelType::Empty);
					}
				}
			}
		}
		if (pair.second) {
			glm::vec3 pos = tanks[0]->getPosition();
			if (pow(pos.x-expl.getX(),2)+ pow(pos.y - expl.getY(), 2)+ pow(pos.z + expl.getZ(), 2)<pow(tankRadius+explRadius,2)) {
				game->tankGotHit(0);
			}
		}
		else {
			glm::vec3 pos = tanks[1]->getPosition();
			if (pow(pos.x - expl.getX(), 2) + pow(pos.y - expl.getY(), 2) + pow(pos.z + expl.getZ(), 2)<pow(tankRadius + explRadius, 2)){
				game->tankGotHit(1);
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
		if (colObj0Wrap->getCollisionObject()->getUserIndex() == 7) {
			Tank::Bullet* bullet = ((Tank::Bullet*)colObj0Wrap->getCollisionObject()->getUserPointer());
			bullet->disableMe = true;
			std::cout << cp.getPositionWorldOnA().getX() << " " << cp.getPositionWorldOnA().getY() << " " << cp.getPositionWorldOnA().getZ() << "\n";
			//show aim circle at collision position
			return false;
		}
		if (colObj1Wrap->getCollisionObject()->getUserIndex() == 7) {
			Tank::Bullet* bullet = ((Tank::Bullet*)colObj1Wrap->getCollisionObject()->getUserPointer());
			bullet->disableMe = true;
			std::cout << cp.getPositionWorldOnB().getX() << " " << cp.getPositionWorldOnB().getY() << " " << cp.getPositionWorldOnB().getZ() << "\n";
			//show aim circle at collision position
			return false;
		}
		if (colObj0Wrap->getCollisionObject()->getUserIndex() == 10) {
			Tank::Bullet* bullet = ((Tank::Bullet*)colObj0Wrap->getCollisionObject()->getUserPointer());
			bullet->disableMe = true;
			explosionHandler->addExplosionPoint(cp.getPositionWorldOnA(), bullet->owner);
		}
		if (colObj1Wrap->getCollisionObject()->getUserIndex() == 10) {
			Tank::Bullet* bullet = ((Tank::Bullet*)colObj1Wrap->getCollisionObject()->getUserPointer());
			bullet->disableMe = true;
			explosionHandler->addExplosionPoint(cp.getPositionWorldOnB(), bullet->owner);
		}
		return false;
	}
}