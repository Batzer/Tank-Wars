#include "Tank.h"
#include <glm/gtc/type_ptr.hpp>
namespace tankwars {

	Tank::Tank(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, btVector3 startingPosition,int tankID)
		: wheelDirection(0, -1, 0),
		  wheelAxle(-1, 0, 0),
		  renderer(&renderer),
		  dynamicsWorld(dynamicsWorld),
		  bulletHandler(dynamicsWorld, renderer,tankID),
		  tankTuning(),
		  tankBoxShape(new btBoxShape(btVector3(1.f, .5f, 2.f))),
		  tankID(tankID)
	{
		//setTankTuning();
		//tr.setIdentity();

		tankMotionState.reset(new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), startingPosition)));

        // Use a compound shape so we can set a different center of mass.
        // (0, 1, 0) means that the center of mass is at (0, -1, 0) which makes the tank more stable.
        compoundShape.reset(new btCompoundShape);
        compoundShape->addChildShape(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 1, 0)), tankBoxShape.get());

		btVector3 localInertia;
        compoundShape->calculateLocalInertia(mass, localInertia);
        btRigidBody::btRigidBodyConstructionInfo tankRigidBodyCI(mass, tankMotionState.get(), compoundShape.get(), localInertia);

		tankChassis.reset(new btRigidBody(tankRigidBodyCI));
		dynamicsWorld->addRigidBody(tankChassis.get());
		tankVehicleRaycaster.reset(new btDefaultVehicleRaycaster(dynamicsWorld));
		tank.reset(new btRaycastVehicle(tankTuning, tankChassis.get(), tankVehicleRaycaster.get()));
		tank->setCoordinateSystem(0, 1, 2);
		tankChassis->setActivationState(DISABLE_DEACTIVATION);
		dynamicsWorld->addVehicle(tank.get());
		addWheels();

		initializeTankMeshInstances(startingPosition);
	}


	void Tank::initializeTankMeshInstances(btVector3 startPos) {
		//Material
		tankMaterial.diffuseColor = { 0.6f, 0.6f, 0 };
		tankMaterial.specularColor = { 1, 1, 0 };
		tankMaterial.specularExponent = 16;


		auto tankBodyModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankBody.obj");
		auto tankHeadModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankHeadCentered.obj");
		auto tankCanonModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankShootingThingCentered.obj");
		auto tankLeftFrontWheelModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankLeftFrontWheelCentered.obj");
		auto tankRightFrontWheelModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankRightFrontWheelCentered.obj");
		auto tankLeftBackWheelModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankLeftBackWheelCentered.obj");
		auto tankRightBackWheelModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankRightBackWheelCentered.obj");
		//Meshes
		tankBodyMesh.reset(new Mesh(tankwars::createMeshFromWavefront(tankBodyModel)));
		tankHeadMesh.reset(new Mesh(tankwars::createMeshFromWavefront(tankHeadModel)));
		tankCanonMesh.reset(new Mesh(tankwars::createMeshFromWavefront(tankCanonModel)));
		tankLeftFrontWheelMesh.reset(new Mesh(tankwars::createMeshFromWavefront(tankLeftFrontWheelModel)));
		tankRightFrontWheelMesh.reset(new Mesh(tankwars::createMeshFromWavefront(tankRightFrontWheelModel)));
		tankLeftBackWheelMesh.reset(new Mesh(tankwars::createMeshFromWavefront(tankLeftBackWheelModel)));
		tankRightBackWheelMesh.reset(new Mesh(tankwars::createMeshFromWavefront(tankRightBackWheelModel)));

		//Transform
		tankModelMat = glm::translate(glm::mat4(1), glm::vec3(startPos.getX(), startPos.getY(), startPos.getZ()));
		tankModelMat = glm::scale(tankModelMat, glm::vec3(8, 8, 8));
		tankModelMat = glm::rotate(tankModelMat, glm::pi<float>(), glm::vec3(0, 1, 0));

		//MeshInstances
        tankMeshInstances.reserve(7);
		tankMeshInstances.emplace_back(*tankBodyMesh, tankMaterial);
		tankMeshInstances.emplace_back(*tankHeadMesh, tankMaterial);
		tankMeshInstances.emplace_back(*tankCanonMesh, tankMaterial);
		tankMeshInstances.emplace_back(*tankRightFrontWheelMesh, tankMaterial);
		tankMeshInstances.emplace_back(*tankLeftFrontWheelMesh, tankMaterial);
		tankMeshInstances.emplace_back(*tankRightBackWheelMesh, tankMaterial);
		tankMeshInstances.emplace_back(*tankLeftBackWheelMesh, tankMaterial);

		for (int i = 0; i < 7; i++) { //wheels not there yet
			tankMeshInstances[i].modelMatrix = tankModelMat;
			renderer->addSceneObject(tankMeshInstances[i]);
		}
	}

	btRaycastVehicle* Tank::getAction() {
		return tank.get();
	}

	btRigidBody* Tank::getRigidBody() {
		return tankChassis.get();
	}

	MeshInstance* Tank::getTankMeshInstance(int i) { // how to best return all of the instances?
		return &tankMeshInstances[i];
	}

	void Tank::setTankTuning() {
		tankTuning.m_frictionSlip = 10.5f;
		tankTuning.m_maxSuspensionForce = 100;
		tankTuning.m_maxSuspensionTravelCm = 100;
		tankTuning.m_suspensionCompression = 100;
		tankTuning.m_suspensionDamping = 100;
		tankTuning.m_suspensionStiffness = 5.88f;
	}

	void Tank::addWheels() {
		btVector3  connectionPointCSO(1.f, .4f, -1.8f);//0.5 - (0.3*wheelWidth), 1.2f, -2 * 1 + backWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, true);

		connectionPointCSO = btVector3(-1.f, .4f, -1.8f);//-0.5 + (0.3*wheelWidth), 1.2f, -2 * 1 + backWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, true);

		connectionPointCSO = btVector3(1.f, .4f, 1.8f);//-0.5 + (0.3*wheelWidth), 1.2f, 2 * 1 - frontWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, frontWheelRadius, tankTuning, false);

		connectionPointCSO = btVector3(-1.f, .4f, 1.8f);//0.5 - (0.3*wheelWidth), 1.2f, 2 * 1 - frontWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, frontWheelRadius, tankTuning, false);

		for (int i = 0; i < 4; i++) {
			tank->getWheelInfo(i).m_suspensionStiffness = 20.f;
			tank->getWheelInfo(i).m_wheelsDampingRelaxation = 2.3f;
			tank->getWheelInfo(i).m_wheelsDampingCompression = 4.4f;
			tank->getWheelInfo(i).m_frictionSlip = 1000;
			tank->getWheelInfo(i).m_rollInfluence = 01.f;
		}
	}

	glm::vec3 Tank::getPosition() {
		btTransform trans;
		tankChassis->getMotionState()->getWorldTransform(trans);
		btVector3 pos = trans.getOrigin();
		return glm::vec3(float(pos.getX()), float(pos.getY()), float(pos.getZ()));
	}

	glm::vec3 Tank::getDirectionVector() {
		return -glm::vec3(tankMeshInstances[1].modelMatrix[2][0], 0, tankMeshInstances[1].modelMatrix[2][2]);
	}

	glm::vec3 Tank::offset() {
		btTransform trans;
		tankChassis->getMotionState()->getWorldTransform(trans);
		glm::mat4 bulletMatrix(0.f);
		trans.getOpenGLMatrix(glm::value_ptr(bulletMatrix));
		return glm::vec3(bulletMatrix[3][0], bulletMatrix[3][1], bulletMatrix[3][2]);
	}
	int Tank::getPoints() {
		return points;
	}
	void Tank::addPoint() {
		points++;
	}
	

	//-------------------------------------------Controller-Functions----------------------------

	void Tank::reset(glm::vec3 pos,glm::vec3 lookAt) {
		lookAt.y = 0;
		tankEngineForce = 0;
		tankBreakingForce = 0;
		tankChassis->clearForces();
		btTransform trans;
		glm::mat4 model;
		model[0][0] = 1;
		model[1][1] = 1;
		model[2][2] = 1;
		model[3][3] = 1;
		model = glm::translate(model, pos);
		lookAt = glm::normalize(lookAt);
		btScalar angle = glm::acos(glm::dot(lookAt,glm::vec3(model[2][0], model[2][1], model[2][2])));
		model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
		tankModelMat = model;
		trans.setFromOpenGLMatrix(glm::value_ptr(tankModelMat));
		tankChassis->getMotionState()->setWorldTransform(trans);
		tankChassis->setMotionState(tankMotionState.get());
		tankChassis->setLinearVelocity(btVector3(0, 0, 0));
		tankChassis->setAngularVelocity(btVector3(0, 0, 0));
		turretAngle = 0;
		headAndTurretAngle = 0;
		tankBreakingForce = maxBreakingForce;
	}
	
	void Tank::turnTurretController(float val) {
		if (val > 0) {
			if (turretAngle<turretMaxAngle)
				turretAngle += val*turretRotationAlpha;
		}
		else {
			if (turretAngle > turretMinAngle)
				turretAngle += val*turretRotationAlpha;
		}
	}

	void Tank::turnHeadAndTurretController(float val) {
		headAndTurretAngle -= val*headAndTurretRotationAlpha;
		if (headAndTurretAngle > glm::pi<float>())
			headAndTurretAngle -= 2 * glm::pi<float>();
		if (headAndTurretAngle < -glm::pi<float>())
			headAndTurretAngle += 2 * glm::pi<float>();
	}

	void Tank::breakController() {
		tankEngineForce = 0;
		tankBreakingForce = maxBreakingForce;
	}

	void Tank::driveController(bool forward) {
		tankBreakingForce = defaultBreakingForce; // or maybe 0
		if (forward) {
			tankEngineForce = -maxEngineForce;
			//tankBreakingForce = defaultBreakingForce;				????????????????-----------------------------------------------
		}
		else {
			tankEngineForce = (maxEngineForce / 2);
			//tankBreakingForce = defaultBreakingForce;				??????????????????????????????????---------------------------------			
		}
	}

	void Tank::turnController(float val) {
		val = -val;
		tankSteering = steeringClamp*val;
	}

	void Tank::shoot(float dt) {
		if (dt - lastTimeShot > timeBetweenShots) {
			btTransform trans;
			trans.setFromOpenGLMatrix(glm::value_ptr(tankMeshInstances[2].modelMatrix));
			bulletHandler.createNewBullet(trans);
			lastTimeShot = dt;
		}
	}
	void Tank::adjustPower(bool increase, float dt) {
		if (dt - lastPowerAdjust > timeBetweenPowerAdjusts) {
			if (increase) {
				if (shootingPower < shootingPowerMax)
					shootingPower += shootingPowerIncrease;
			}
			else {
				if (shootingPower > shootingPowerMin)
					shootingPower -= shootingPowerIncrease;
			}
			lastPowerAdjust = dt;
			bulletHandler.updatePower(shootingPower);
		}
	}
	//---------------------------------------End-Controller-Functions----------------------------

	void Tank::turn(bool left) {
		if (!left) {
			tankSteering += steeringIncrement;
			if (tankSteering > steeringClamp) {
				tankSteering = steeringClamp;
			}
		}
		else {
			tankSteering -= steeringIncrement;
			if (tankSteering < -steeringClamp) {
				tankSteering = -steeringClamp;
			}
		}
	}

	void Tank::drive(bool forward) {
		if (forward) {
			tankEngineForce = maxEngineForce;
			tankBreakingForce = 0.f;
		}
		else {
			tankEngineForce = 0.f;
			tankBreakingForce = defaultBreakingForce;
		}
	}

	void Tank::driveBack(bool backward) {
		if (backward) {
			tankEngineForce = -maxEngineForce / 2;
			tankBreakingForce = 0.f;
		}
		else {
			tankEngineForce = 0.f;
			tankBreakingForce = defaultBreakingForce;
		}
	}

	void Tank::update(float dt) {
		if (dt - lastTimeEngineDecreased > timeBetweenEngineDecreases) {
			if (tankEngineForce > 0) {
				tankEngineForce -= engineForceReduceFactor;
				if (tankEngineForce < 0) {
					tankEngineForce = 0;
				}
			}
			else if (tankEngineForce < 0) {
				tankEngineForce += engineForceReduceFactor;
				if (tankEngineForce > 0) {
					tankEngineForce = 0;
				}
			}
			lastTimeEngineDecreased = dt;
		}
		tank->resetSuspension();
		for (int i = 0; i < 4; i++) {
		tank->updateWheelTransform(i, true);
		}

        tank->applyEngineForce(tankEngineForce, 0);
        tank->setBrake(tankBreakingForce, 0);
        tank->applyEngineForce(tankEngineForce, 1);
        tank->setBrake(tankBreakingForce, 1);

		tank->applyEngineForce(tankEngineForce, 2);
		tank->setBrake(tankBreakingForce, 2);
		tank->applyEngineForce(tankEngineForce, 3);
		tank->setBrake(tankBreakingForce, 3);
		tank->setSteeringValue(tankSteering, 0);
		tank->setSteeringValue(tankSteering, 1);

		btTransform trans;
		tankChassis->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(glm::value_ptr(tankModelMat));

		for (MeshInstance& mI : tankMeshInstances) {
			mI.modelMatrix = tankModelMat;
		}
		glm::vec3 rightVec = glm::normalize(glm::vec3(tankModelMat[0][0], 0, tankModelMat[0][2]));
		glm::vec3 upVec = glm::normalize(glm::vec3(tankModelMat[1][0], tankModelMat[1][1], tankModelMat[1][2]));
		tankMeshInstances[1].modelMatrix = glm::translate(glm::rotate(tankModelMat, headAndTurretAngle, glm::vec3(0,1,0)),glm::vec3(0,2,0));//HeadAndCanonRotationAngle 
		tankMeshInstances[2].modelMatrix = glm::translate(glm::rotate(tankMeshInstances[1].modelMatrix, turretAngle, glm::vec3(1, 0, 0)), glm::vec3(0, 0, -1));

		for (int i = 0; i < 4; i++) {
			tank->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(glm::value_ptr(tankModelMat));
			tankMeshInstances[i + 3].modelMatrix = tankModelMat;
		}
		btTransform transi;
		transi.setFromOpenGLMatrix(glm::value_ptr(tankMeshInstances[2].modelMatrix));
		bulletHandler.updateBullets(dt,transi);
	}

	Tank::BulletHandler::BulletHandler(btDynamicsWorld* dynamicsWorld, Renderer& renderer,int tankId)
            : dynamicsWorld(dynamicsWorld),
			  renderer(renderer),
			  bulletMesh(createSphereMesh(0.1f, 5, 5)),
			  bulletShape(0.1f), 
			  tankID(tankId) {
		bulletMat.diffuseColor = { 0.6f, 0.6f, 0 };
		bulletMat.specularColor = { 1, 0, 0 };
		bulletMat.specularExponent = 16;
		bulletInertia = btVector3(0, 0, 0);
		for (int i = 0; i < bulletMax;i++) {
			bullets.at(i).set(tankId, MeshInstance(bulletMesh, bulletMat));
		}
		for (int i = 0; i < bulletRaycastMax; i++) {
			raycastBullets.at(i).set(tankId, MeshInstance(bulletMesh, bulletMat));
			renderer.addSceneObject(raycastBullets.at(i).bulletMeshInstance);
		}
	}
	void Tank::BulletHandler::updatePower(btScalar pwr) {
		power = pwr;
	}
	void Tank::BulletHandler::createNewBullet(btTransform& tr) {
		glm::mat4 bulletMatrix;
		tr.getOpenGLMatrix(glm::value_ptr(bulletMatrix));

		for (int i = 0; i < bulletMax; i++) {
			if (!bullets.at(i).active) {
				bullets.at(i).set(new btRigidBody(mass, new btDefaultMotionState(tr), &bulletShape, bulletInertia));
				bullets.at(i).bulletBody->setLinearVelocity(-btVector3(bulletMatrix[2][0], bulletMatrix[2][1], bulletMatrix[2][2])*power);
				bullets.at(i).bulletBody->setCollisionFlags(bullets.at(i).bulletBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
				bullets.at(i).bulletBody->setUserIndex(10);
				bullets.at(i).bulletBody->setUserPointer(&bullets.at(i));
				bullets.at(i).bulletBody->setCcdMotionThreshold(0.2f);
				bullets.at(i).bulletBody->setCcdSweptSphereRadius(0.1f);
				bullets.at(i).active = true;
				bullets.at(i).disableMe = false;
				dynamicsWorld->addRigidBody(bullets.at(i).bulletBody.get());
				renderer.addSceneObject(bullets.at(i).bulletMeshInstance);
				return;
			}
		}
	}

	void Tank::BulletHandler::updateBullets(btScalar dt,btTransform direction) {
		glm::mat4 bulletMat;
		btTransform trans;
		bool shotABulletThisTick = false;
		for (int i = 0; i < bulletMax;i++) {
			if (bullets.at(i).disableMe) {
				removeBullet(i);
			}
			else if(bullets.at(i).active){
				bullets.at(i).bulletBody->setUserPointer(&bullets.at(i));
				bullets.at(i).bulletBody->getMotionState()->getWorldTransform(trans);
				trans.getOpenGLMatrix(glm::value_ptr(bulletMat));
				bullets.at(i).bulletMeshInstance.modelMatrix = bulletMat;
				shotABulletThisTick = true;
			}
		}
		for (int i = 0; i < bulletRaycastMax; i++) {
			if (raycastBullets[i].disableMe) {
				removeRaycastBullet(i);
			}
		}
		if (!shotABulletThisTick && (dt-lastTimeBulletRaycastShot>timeBetweenBulletRaycastShots)) {
			glm::mat4 bulletMatrix;
			direction.getOpenGLMatrix(glm::value_ptr(bulletMatrix));
			bool foundASpot = false;
			for (int i = 0; !foundASpot || i >= bulletRaycastMax;i++) {
				if (raycastBullets.at(i).active == false) {
					raycastBullets.at(i).set(new btRigidBody(mass, new btDefaultMotionState(direction), &bulletShape, bulletInertia));
					raycastBullets.at(i).bulletBody->setLinearVelocity(-btVector3(bulletMatrix[2][0], bulletMatrix[2][1], bulletMatrix[2][2])*power);
					raycastBullets.at(i).bulletBody->setCollisionFlags(raycastBullets.at(i).bulletBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
					raycastBullets.at(i).bulletBody->setUserIndex(7);
					raycastBullets.at(i).bulletBody->setUserPointer(&raycastBullets.at(i));
					//raycastBullets.at(i).bulletBody->setCcdMotionThreshold(0.2f);
					//raycastBullets.at(i).bulletBody->setCcdSweptSphereRadius(0.1f);
					raycastBullets.at(i).active = true;
					raycastBullets.at(i).disableMe = false;
					dynamicsWorld->addRigidBody(raycastBullets.at(i).bulletBody.get());
					lastTimeBulletRaycastShot = dt;
					foundASpot = true;
				}
			}
		}
	}

	void Tank::BulletHandler::removeBullet(int index) {
		dynamicsWorld->removeRigidBody(bullets.at(index).bulletBody.get());
		renderer.removeSceneObject(bullets.at(index).bulletMeshInstance);
		bullets.at(index).active = false;
	}
	void Tank::BulletHandler::removeRaycastBullet(int index) {
		dynamicsWorld->removeRigidBody(raycastBullets.at(index).bulletBody.get());
		raycastBullets.at(index).active = false;

	}
	Tank::BulletHandler::~BulletHandler() {
        for (auto& bullet : bullets) {
            if (bullet.bulletBody) {
                dynamicsWorld->removeRigidBody(bullet.bulletBody.get());
            }
        }

        for (auto& bullet : raycastBullets) {
            if (bullet.bulletBody) {
                dynamicsWorld->removeRigidBody(bullet.bulletBody.get());
            }
        }
	}
}