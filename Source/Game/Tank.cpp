#include "Tank.h"
#include <glm/gtc/type_ptr.hpp>
namespace tankwars {

	Tank::Tank(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, btVector3 startingPosition)
		:wheelDirection(0, -1, 0),
		wheelAxle(1, 0, 0),
		renderer(renderer),
		//boxMesh(createBoxMesh(2, 1, 1)),
		dnmcWorld(dynamicsWorld),
		bulletHandler(dnmcWorld, renderer),
		//startingPosition(startingPosition), 
		tankTuning(),
		tankBoxShape(new btBoxShape(btVector3(1.f, .5f, 2.f)))
		//tankBodyMesh(createBoxMesh(2,1,4))
	{
		//setTankTuning();
		//tr.setIdentity();
		//tr.setOrigin(startingPosition);
		//tankMotionState  = new btDefaultMotionState(tr);
		//tankChassis = new btRigidBody(mass, tankMotionState, tankBoxShape);
		tr.setIdentity();
		tr.setOrigin(startingPosition);
		tankMotionState = new btDefaultMotionState(tr);

        // Use a compound shape so we can set a different center of mass.
        // (0, 1, 0) means that the center of mass is at (0, -1, 0) which makes the tank more stable.
        compoundShape = new btCompoundShape();
        compoundShape->addChildShape(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 1, 0)), tankBoxShape);

		btVector3 localInertia(0, 0, 0);
		//tankBoxShape->calculateLocalInertia(mass, localInertia);
        //btRigidBody::btRigidBodyConstructionInfo tankRigidBodyCI(mass, tankMotionState, tankBoxShape, localInertia);
        compoundShape->calculateLocalInertia(mass, localInertia);
        btRigidBody::btRigidBodyConstructionInfo tankRigidBodyCI(mass, tankMotionState, compoundShape, localInertia);

		tankChassis = new btRigidBody(tankRigidBodyCI);
		dynamicsWorld->addRigidBody(tankChassis);
		tankVehicleRaycaster = new btDefaultVehicleRaycaster(dnmcWorld);
		tank = new btRaycastVehicle(tankTuning, tankChassis, tankVehicleRaycaster);
		tank->setCoordinateSystem(0, 1, 2);
		tankChassis->setActivationState(DISABLE_DEACTIVATION);
		dnmcWorld->addVehicle(tank);
		addWheels();

		//mat.diffuseColor = { 1,0,0 };
		initializeTankMeshInstances(startingPosition);
	}


	void Tank::initializeTankMeshInstances(btVector3 startPos) {
		//Material
		tankMaterial.diffuseColor = { 0.6f, 0.6f, 0 };
		tankMaterial.specularColor = { 1, 1, 0 };
		tankMaterial.specularExponent = 16;
		//Wavefronts
		tankBodyModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankBody.obj");
		tankHeadModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankHead.obj");
		tankCanonModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankShootingThing.obj");

		tankLeftFrontWheelModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankLeftFrontWheel.obj");
		tankRightFrontWheelModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankRightFrontWheel.obj");
		tankLeftBackWheelModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankLeftBackWheel.obj");
		tankRightBackWheelModel = tankwars::readWavefrontFromFile("Content/Animations/TankObj/TankRightBackWheel.obj");
		//Meshes
		tankBodyMesh = new Mesh(tankwars::createMeshFromWavefront(tankBodyModel));
		tankHeadMesh = new Mesh(tankwars::createMeshFromWavefront(tankHeadModel));
		tankCanonMesh = new Mesh(tankwars::createMeshFromWavefront(tankCanonModel));
		tankLeftFrontWheelMesh = new Mesh(tankwars::createMeshFromWavefront(tankLeftFrontWheelModel));
		tankRightFrontWheelMesh = new Mesh(tankwars::createMeshFromWavefront(tankRightFrontWheelModel));
		tankLeftBackWheelMesh = new Mesh(tankwars::createMeshFromWavefront(tankLeftBackWheelModel));
		tankRightBackWheelMesh = new Mesh(tankwars::createMeshFromWavefront(tankRightBackWheelModel));
		//Transform
		tankModelMat = glm::translate(glm::mat4(1), glm::vec3(startPos.getX(), startPos.getY(), startPos.getZ()));
		tankModelMat = glm::scale(tankModelMat, glm::vec3(8, 8, 8));
		tankModelMat = glm::rotate(tankModelMat, glm::pi<float>(), glm::vec3(0, 1, 0));
		//MeshInstances
		tankMeshInstances.push_back(MeshInstance(*tankBodyMesh, tankMaterial));
		tankMeshInstances.push_back(MeshInstance(*tankHeadMesh, tankMaterial));
		tankMeshInstances.push_back(MeshInstance(*tankCanonMesh, tankMaterial));

		tankMeshInstances.push_back(MeshInstance(*tankLeftFrontWheelMesh, tankMaterial));
		tankMeshInstances.push_back(MeshInstance(*tankRightFrontWheelMesh, tankMaterial));
		tankMeshInstances.push_back(MeshInstance(*tankLeftBackWheelMesh, tankMaterial));
		tankMeshInstances.push_back(MeshInstance(*tankRightBackWheelMesh, tankMaterial));


		tankMeshInstances.resize(7);
		for (int i = 0; i < 7; i++) {//wheels not there yet
			tankMeshInstances[i].modelMatrix = tankModelMat;
			renderer.addSceneObject(tankMeshInstances[i]);
		}

	}
	btRaycastVehicle* Tank::getAction() {
		return tank;
	}
	btRigidBody* Tank::getRigidBody() {
		return tankChassis;
	}
	MeshInstance Tank::getTankMeshInstance(int i) { // how to best return all of the instances?
		return tankMeshInstances[i];
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
		btVector3 connectionPointCSO(-1.f, .4f, -1.8f);//-0.5 + (0.3*wheelWidth), 1.2f, -2 * 1 + backWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, true);

		connectionPointCSO = btVector3(1.f, .4f, -1.8f);//0.5 - (0.3*wheelWidth), 1.2f, -2 * 1 + backWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, true);

		connectionPointCSO = btVector3(-1.f, .4f, 1.8f);//0.5 - (0.3*wheelWidth), 1.2f, 2 * 1 - frontWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, frontWheelRadius, tankTuning, false);

		connectionPointCSO = btVector3(1.f, .4f, 1.8f);//-0.5 + (0.3*wheelWidth), 1.2f, 2 * 1 - frontWheelRadius);
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
		btTransform trans;
		tankChassis->getMotionState()->getWorldTransform(trans);
		glm::mat4 bulletMatrix(0.f);
		trans.getOpenGLMatrix(glm::value_ptr(bulletMatrix));
		bulletMatrix = glm::rotate(bulletMatrix, headAndTurretAngle, glm::vec3(0, 1, 0));
		return glm::vec3(-bulletMatrix[2][0],0 , -bulletMatrix[2][2]);//-bulletMatrix[2][1]
	}
	glm::vec3 Tank::offset() {
		btTransform trans;
		tankChassis->getMotionState()->getWorldTransform(trans);
		glm::mat4 bulletMatrix(0.f);
		trans.getOpenGLMatrix(glm::value_ptr(bulletMatrix));
		return glm::vec3(bulletMatrix[3][0], bulletMatrix[3][1], bulletMatrix[3][2]);
	}
	//tank->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);

	//-------------------------------------------Controller-Functions----------------------------
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
		tankBreakingForce = defaultBreakingForce;
	}

	void Tank::driveController(float val) {
		if (val <= 0) {
			tankEngineForce = -maxEngineForce*val;
		}
		else {
			tankEngineForce = -(maxEngineForce / 2)*val;
		}
	}

	void Tank::turnController(float val) {
		val = -val;
		tankSteering = steeringClamp*val;
	}

	void Tank::shoot() {
		btTransform trans;
		tankChassis->getMotionState()->getWorldTransform(trans);
		bulletHandler.createNewBullet(trans, headAndTurretAngle, turretAngle, shootingPower);
	}

	void Tank::adjustPower(bool increase) {
		if (increase) {
			if (shootingPower < shootingPowerMax)
				shootingPower += shootingPowerIncrease;
		}
		else {
			if (shootingPower > 0)
				shootingPower -= shootingPowerIncrease;
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
	void Tank::update() {
		tank->resetSuspension();
		for (int i = 0; i < 4; i++) {
		tank->updateWheelTransform(i, true);
		//if (tank->getWheelInfo(i).m_raycastInfo.m_groundObject)
		//std::cout << i << " ";								//Doesn't touch the fucking ground!
		}
		std::cout << "\n";
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

		glm::vec3 rightVec(tankModelMat[0][0], 0, tankModelMat[0][2]);
		glm::vec3 upVec(tankModelMat[1][0], tankModelMat[1][1], tankModelMat[1][2]);
		tankMeshInstances[1].modelMatrix = glm::rotate(tankModelMat, headAndTurretAngle, upVec);//HeadAndCanonRotationAngle 
		tankMeshInstances[2].modelMatrix = glm::rotate(glm::rotate(tankModelMat, headAndTurretAngle, upVec), turretAngle, glm::normalize(rightVec));
		for (int i = 0; i < 4; i++) {
			tank->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(glm::value_ptr(tankModelMat));
			tankMeshInstances[i + 3].modelMatrix = tankModelMat;
		}

		bulletHandler.updateBullets();

	}

	Tank::BulletHandler::BulletHandler(btDynamicsWorld* dnmcWorld, Renderer& renderer) :
			dnmcWorld(dnmcWorld),
			renderer(renderer),
			bulletMesh(createSphereMesh(0.1f, 5, 5)),
			bulletShape(0.1f) {
		bulletMat.diffuseColor = { 0.6f, 0.6f, 0 };
		bulletMat.specularColor = { 1, 0, 0 };
		bulletMat.specularExponent = 16;
		bullets.reserve(bulletMax);
	}
	void Tank::BulletHandler::createNewBullet(btTransform& tr, btScalar headAngle, btScalar turretAngle, btScalar power) {
		if (bullets.size() >= bulletMax) return;
		glm::mat4 bulletMatrix;
		tr.getOpenGLMatrix(glm::value_ptr(bulletMatrix));
		bulletMatrix[3][1] += 4;										//<---- for testing
		tr.setFromOpenGLMatrix(glm::value_ptr(bulletMatrix));

		btVector3 bulletInertia(0, 0, 0);
		btScalar mass = 20;
		//bulletRigidBodies[bulletCounter] = new btRigidBody(mass, new btDefaultMotionState(tr), &bulletShape, bulletInertia);

		glm::vec4 direction(1, 0, 0, 0);
		bulletMatrix = glm::rotate(bulletMatrix, 1.57f + headAngle, glm::vec3(0, 1, 0));
		glm::vec3 forwardVec(bulletMatrix[2][0], bulletMatrix[2][1], bulletMatrix[2][2]); // there is a 1 instead of the 2 in the z-Parameter
		bulletMatrix = glm::rotate(bulletMatrix, turretAngle, glm::cross(forwardVec, glm::vec3(0, 1, 0)));
		direction = bulletMatrix*direction;
		if (headAngle>0)
			direction = glm::vec4(direction.x, -direction.y, direction.z, 0);
		direction = glm::normalize(direction);
		bullets.emplace_back(new btRigidBody(mass, new btDefaultMotionState(tr), &bulletShape, bulletInertia), MeshInstance(bulletMesh, bulletMat));
		bullets.back().bulletBody->setLinearVelocity(btVector3(direction.x*power, direction.y*power, direction.z*power));
		bullets.back().bulletBody->setCollisionFlags(bullets.back().bulletBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		bullets.back().bulletBody->setUserIndex(10);
		bullets.back().bulletBody->setUserPointer(&bullets.back());
		dnmcWorld->addRigidBody(bullets.back().bulletBody);
		renderer.addSceneObject(bullets.back().bulletMeshInstance);
	}
	void Tank::BulletHandler::updateBullets() {
		glm::mat4 bulletMat;
		btTransform trans;
		for (int i = 0; i < bullets.size();) {
			if (bullets[i].disableMe) {
				removeBullet(bullets[i]);
			}
			else {
				bullets[i].bulletBody->getMotionState()->getWorldTransform(trans);
				trans.getOpenGLMatrix(glm::value_ptr(bulletMat));
				bullets[i].bulletMeshInstance.modelMatrix = bulletMat;
				i++;
			}
		}
	}
	void Tank::BulletHandler::removeBullet(Bullet & bul) {
		dnmcWorld->removeRigidBody(bul.bulletBody);
		renderer.removeSceneObject(bul.bulletMeshInstance);
		std::swap(bul, bullets.back());
		bullets.pop_back();
	}
}