#include "ExplosionHandling.h"

namespace tankwars {
    std::unique_ptr<ExplosionHandler> explosionHandler;

	ExplosionHandler::ExplosionHandler(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, VoxelTerrain& terrain, Tank* tank1, Tank* tank2, Game* game) 
			: game(game), 
			  dnmcWrld(dynamicsWorld), 
			  renderer(renderer), 
			  terrain(terrain),
			  smokeTexture(tankwars::createTextureFromFile("Content/Textures/smoke.png")),
			  smokeParticleSystem(2024,smokeTexture),
			  starYellowTexture(tankwars::createTextureFromFile("Content/Textures/starYellow.png")),
			  starOrangeTexture(tankwars::createTextureFromFile("Content/Textures/starOrange.png")),
			  starYellowParticleSystem(512,starYellowTexture),
			starOrangeParticleSystem(512, starOrangeTexture) {
		tanks[0] = tank1; 
		tanks[1] = tank2; 
		smokeParticleSystem.setParticleColorRange({ 1, 1, 1, 0.25f }, { 1, 1, 1, 0.75f });
		smokeParticleSystem.setParticleSizeRange(1, 3);
		smokeParticleSystem.setParticleLifeTimeRange(3, 6);
		smokeParticleSystem.setEmitterType(EmitterType::Sphere);
		smokeParticleSystem.setEmitterRadius(1);
		smokeParticleSystem.setParticleVelocityRange(glm::vec3(-4,-4,-4), glm::vec3(4,4,4));
		smokeParticleSystem.setParticleAccelerationRange(glm::vec3(), glm::vec3());
		renderer.addParticleSystem(smokeParticleSystem);

		starYellowParticleSystem.setParticleColorRange({ 1, 1, 1, 0.25f }, { 1, 1, 1, 0.75f });
		starYellowParticleSystem.setParticleSizeRange(0, 0.5);
		starYellowParticleSystem.setParticleLifeTimeRange(3, 4);
		starYellowParticleSystem.setEmitterType(EmitterType::Sphere);
		starYellowParticleSystem.setEmitterRadius(2);
		renderer.addParticleSystem(starYellowParticleSystem);

		starOrangeParticleSystem.setParticleColorRange({ 1, 1, 1, 0.25f }, { 1, 1, 1, 0.75f });
		starOrangeParticleSystem.setParticleSizeRange(0.5, 1);
		starOrangeParticleSystem.setEmitterType(EmitterType::Sphere);
		starOrangeParticleSystem.setEmitterRadius(0.5);
		starOrangeParticleSystem.setParticleLifeTimeRange(3, 4);
		renderer.addParticleSystem(starOrangeParticleSystem);
	};

    ExplosionHandler::~ExplosionHandler() {
        glDeleteTextures(1, &smokeTexture);
    }

    void ExplosionHandler::update(btScalar dt) {
		handleExplosions();
		smokeParticleSystem.update(dt, [&](tankwars::Particle& p){
			btScalar val = (std::pow(p.velocity[0], 2) + std::pow(p.velocity[1], 2) + std::pow(p.velocity[2], 2))*0.002;
			if (p.velocity[0] < 0) {
				p.velocity[0] += val;
			}
			else {
				p.velocity[0] -= val;
			}
			if (p.velocity[1] < 0) {
				p.velocity[1] += val;
			}
			else {
				p.velocity[1] -= val;
			}
			if (p.velocity[2] < 0) {
				p.velocity[2] += val;
			}
			else {
				p.velocity[2] -= val;
			}
			//p.velocity[1] -= (std::pow(p.velocity[0], 2) + std::pow(p.velocity[1], 2) + std::pow(p.velocity[2], 2))*0.001;
			//p.velocity[2] -= (std::pow(p.velocity[0], 2) + std::pow(p.velocity[1], 2) + std::pow(p.velocity[2], 2))*0.001;
			/*p.color.a -= 0.1f * dt;
			if (p.color.a < 0.0f) {
				p.isAlive = false;
			}*/
		});
		starYellowParticleSystem.update(dt, [&](tankwars::Particle& p) {
			if(p.size<2)
			p.size += 0.001;
			/*p.color.a -= 0.4f * dt;
			if (p.color.a < 0.0f) {
				p.isAlive = false;
			}*/
		});
		starOrangeParticleSystem.update(dt, [&](tankwars::Particle& p) {
			if (p.size < 2.5)
				p.size += 0.003;
			/*p.color.a -= 0.3f * dt;
			if (p.color.a < 0.0f) {
				p.isAlive = false;
			}*/
		});

	}

	void ExplosionHandler::addExplosionPoint(btVector3 explosionAt,int owner) {
		explosionPoints.push_back(std::make_pair(explosionAt,owner));
	}

	void ExplosionHandler::explosion(std::pair<btVector3,int> pair) {
		smokeParticleSystem.setEmitterPosition(glm::vec3(pair.first.getX(), pair.first.getY(), pair.first.getZ()));
		starYellowParticleSystem.setEmitterPosition(glm::vec3(pair.first.getX(), pair.first.getY(), pair.first.getZ()));
		starOrangeParticleSystem.setEmitterPosition(glm::vec3(pair.first.getX(), pair.first.getY(), pair.first.getZ()));
		starYellowParticleSystem.emit(60);
		smokeParticleSystem.emit(500);
		
		starYellowParticleSystem.emit(10);
		
		starOrangeParticleSystem.emit(5);
		btVector3 expl(pair.first.getX(), pair.first.getY(), -pair.first.getZ());
		int xMin = std::max((int)(expl.getX() - explRadius), 1);
		int yMin = std::max((int)(expl.getY() - explRadius), 1);
		int zMin = std::max((int)(expl.getZ() - explRadius), 1);
		int xMax = std::min((int)(expl.getX() + explRadius + 0.5), (int)terrain.getWidth()-3);
		int yMax = std::min((int)(expl.getY() + explRadius + 0.5), (int)terrain.getHeight()-1);
		int zMax = std::min((int)(expl.getZ() + explRadius + 0.5), (int)terrain.getDepth()-3);
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
			//std::cout << cp.getPositionWorldOnA().getX() << " " << cp.getPositionWorldOnA().getY() << " " << cp.getPositionWorldOnA().getZ() << "\n";
			//show aim circle at collision position
			return false;
		}
		if (colObj1Wrap->getCollisionObject()->getUserIndex() == 7) {
			Tank::Bullet* bullet = ((Tank::Bullet*)colObj1Wrap->getCollisionObject()->getUserPointer());
			bullet->disableMe = true;
			//std::cout << cp.getPositionWorldOnB().getX() << " " << cp.getPositionWorldOnB().getY() << " " << cp.getPositionWorldOnB().getZ() << "\n";
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