#include "Tank.h"
#include <glm/gtc/type_ptr.hpp>
namespace tankwars {

	Tank::Tank(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, btVector3 startingPosition)
			:wheelDirection(0, -1, 0),
			wheelAxle(-1, 0, 0),
			renderer(renderer),
			//boxMesh(createBoxMesh(2, 1, 1)),
			dnmcWorld(dynamicsWorld), 
			//startingPosition(startingPosition), 
			tankTuning(), 
			tankBoxShape(new btBoxShape(btVector3(1.f, 0.5f, 2.f))) {
		//setTankTuning();
		//tr.setIdentity();
		//tr.setOrigin(startingPosition);
		//tankMotionState  = new btDefaultMotionState(tr);
		//tankChassis = new btRigidBody(mass, tankMotionState, tankBoxShape);
		tr.setIdentity();
		tr.setOrigin(startingPosition);
        tankMotionState =
            new btDefaultMotionState(tr);
        btVector3 localInertia(0, 0, 0);
		tankBoxShape->calculateLocalInertia(mass, localInertia);
        btRigidBody::btRigidBodyConstructionInfo tankRigidBodyCI(mass, tankMotionState, tankBoxShape, localInertia);
		
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
		//Meshes
		tankBodyMesh = new Mesh(tankwars::createMeshFromWavefront(tankBodyModel));
		tankHeadMesh = new Mesh(tankwars::createMeshFromWavefront(tankHeadModel));
		tankCanonMesh = new Mesh(tankwars::createMeshFromWavefront(tankCanonModel));
		//Transform
		tankModelMat = glm::translate(glm::mat4(1), glm::vec3(startPos.getX(), startPos.getY(), startPos.getZ()));
		tankModelMat = glm::scale(tankModelMat, glm::vec3(8, 8, 8));
		//MeshInstances
		tankMeshInstances.push_back(MeshInstance(*tankBodyMesh, tankMaterial));
		tankMeshInstances.push_back(MeshInstance(*tankHeadMesh, tankMaterial));
		tankMeshInstances.push_back(MeshInstance(*tankCanonMesh, tankMaterial));
		//tankMeshInstances.resize(7);
		for (int i = 0; i < 3; i++) {//wheels not there yet
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
        tankTuning.m_frictionSlip = 100;
        tankTuning.m_maxSuspensionForce = 100;
        tankTuning.m_maxSuspensionTravelCm = 100;
        tankTuning.m_suspensionCompression = 100;
        tankTuning.m_suspensionDamping = 100;
        tankTuning.m_suspensionStiffness = 200;
    }
   
    void Tank::addWheels() {
		btVector3 connectionPointCSO(1 - (0.3*wheelWidth), 1.2f, 2 * 1 - frontWheelRadius);
        tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle,suspensionRestLength,frontWheelRadius, tankTuning, true);
		
		connectionPointCSO = btVector3(-1 + (0.3*wheelWidth), 1.2f, 2 * 1 - frontWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, frontWheelRadius, tankTuning, true);
		
		connectionPointCSO = btVector3(-1 + (0.3*wheelWidth), 1.2f, 2 * 1 - backWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, false);
		
		connectionPointCSO = btVector3(1 - (0.3*wheelWidth), 1.2f, 2 * 1 - backWheelRadius);
		tank->addWheel(connectionPointCSO, wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, false);
        for (int i = 0; i < 4;i++) { 
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
        return glm::vec3(float(pos.getX()),float(pos.getY()),float(pos.getZ()));
    }

			//tank->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);
			
	void Tank::turn(bool left) {
		if (left) {
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
	void Tank::update() {
		//std::cout << tank->getForwardVector().getX()<<" "<< tank->getForwardVector().getY() << " "<< tank->getForwardVector().getZ() << "\n";
		//tank->updateAction(dnmcWorld, 7);
		tank->resetSuspension();
		for (int i = 0; i < 4; i++) {
			tank->updateWheelTransform(i, true);
			if (tank->getWheelInfo(i).m_raycastInfo.m_groundObject)
				std::cout << i << " ";								//Doesn't touch the fucking ground!
		}
		std::cout << "\n";
		//std::cout << tank->getCurrentSpeedKmHour();
		//tank->debugDraw();
		//drive(true);
		/*for (int i = 0; i < 4; i++) {
			tank->updateWheelTransform(i, true);
		}*/
		tank->applyEngineForce(tankEngineForce, 2);
		tank->setBrake(tankBreakingForce,2);
		tank->applyEngineForce(tankEngineForce, 3);
		tank->setBrake(tankBreakingForce, 3);
		tank->setSteeringValue(tankSteering, 0);
		tank->setSteeringValue(tankSteering, 1);
        
		btScalar m[16];
		btTransform trans;
		tankChassis->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(m);
		tankModelMat = *((glm::mat4*) m);
        trans.getOpenGLMatrix(glm::value_ptr(tankModelMat));

		
		/*tankModelMat[0][0] = m[0];
		tankModelMat[1][0] = m[1];
		tankModelMat[2][0] = m[2];

		tankModelMat[0][1] = m[4];
		tankModelMat[1][1] = m[5];
		tankModelMat[2][1] = m[6];

		tankModelMat[0][2] = m[8];
		tankModelMat[1][2] = m[9];
		tankModelMat[2][2] = m[10];

		tankModelMat[0][3] = m[12];
		tankModelMat[1][3] = m[13];
		tankModelMat[2][3] = m[14];*/ // well that's wrong:/ need to get m to a matrix
		//tankModelMat = glm::translate(tankModelMat, glm::vec3(0.01,0,-0.01));  //slowly drifting away
		//tankModelMat = glm::rotate(tankModelMat, 0.01f, glm::vec3(0, 0, 1));
		for (MeshInstance& mI: tankMeshInstances) {
			mI.modelMatrix = tankModelMat;
		}
		/*btMatrix3x3 rotMat = tank->getChassisWorldTransform().getBasis();
		int forwardAxis = tank->getForwardAxis();														// this is the Y Axis for some reason
		glm::vec3 forwardVec(rotMat[0][forwardAxis], rotMat[1][forwardAxis], rotMat[2][forwardAxis]);
		int upAxis = tank->getUpAxis();																	// this is the Z Axis for some reason
		glm::vec3 upVec(rotMat[0][upAxis], rotMat[1][upAxis], rotMat[2][upAxis]);
		*/
		std::cout << "\n";
		glm::vec3 forwardVec(m[0], m[1], m[2]);
		glm::vec3 upVec(m[4], m[5], m[6]);
		//tankMeshInstances[1].modelMatrix = glm::rotate(tankModelMat, headAndCanonRotationAngle, upVec);//HeadAndCanonRotationAngle 
		//tankMeshInstances[2].modelMatrix += glm::rotate(glm::rotate(tankModelMat, headAndCanonRotationAngle, upVec),canonRotationAngle,forwardVec);//HeadAndCanonRotationAngle & CanonRotationAngle / 
		//test rotations
		setHeadAndCanonRotation(0.01f);
		setCanonRotation(0.01f);
	}
	void Tank::setHeadAndCanonRotation(btScalar angle) {
		
		headAndCanonRotationAngle += angle;
	}
	void Tank::setCanonRotation(btScalar angle) {
		//grenzwerte!!!
		canonRotationAngle += angle;
	}
}