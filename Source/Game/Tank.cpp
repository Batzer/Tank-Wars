#include "Tank.h"
namespace tankwars {

	Tank::Tank( btDiscreteDynamicsWorld *dynamicsWorld, btVector3 startingPosition) 
			:wheelDirection(0, -1, 0),
			wheelAxle(-1, 0, 0),
			boxMesh(createBoxMesh(2, 1, 1)),
			dnmcWorld(dynamicsWorld), 
			startingPosition(startingPosition), 
			tankTuning(), 
			tankBoxShape(new btBoxShape(btVector3(2, 1, 1))) {
		tr.setIdentity();
		tr.setOrigin(startingPosition);
		tankMotionState  = new btDefaultMotionState(tr);
		tankChassis = new btRigidBody(mass, tankMotionState, tankBoxShape);
		tank = new btRaycastVehicle(tankTuning, tankChassis, new btDefaultVehicleRaycaster(dnmcWorld));
		tankChassis->setActivationState(DISABLE_DEACTIVATION);
		dnmcWorld->addVehicle(tank);
		addWheels();
        //tankBoxShape->calculateLocalInertia(mass, btVector3(0, 0, 0));
		setTankTuning();
		mat.diffuseColor = { 1,0,0 };
		initializeTankMeshInstances();
    }
	void Tank::initializeTankMeshInstances() {
		TankMeshInstances.resize(7);
		for (int i = 0; i < 7; i++) {
			TankMeshInstances[i].material = &mat;
			TankMeshInstances[i].mesh = &boxMesh;
		}
		//load meshes
		
	}

	MeshInstance Tank::getTankMeshInstance(int i) { // how to best return all of the instances?
		return TankMeshInstances[i];
	}

    void Tank::setTankTuning() {
        tankTuning.m_frictionSlip = 0;
        tankTuning.m_maxSuspensionForce = 0;
        tankTuning.m_maxSuspensionTravelCm = 0;
        tankTuning.m_suspensionCompression = 0;
        tankTuning.m_suspensionDamping = 0;
        tankTuning.m_suspensionStiffness = 200;
    }
   
    void Tank::addWheels() {
        tank->addWheel(btVector3(1, 0, -1), wheelDirection, wheelAxle,suspensionRestLength,frontWheelRadius, tankTuning, true);
		tank->addWheel(btVector3(1, 0, 1), wheelDirection, wheelAxle, suspensionRestLength, frontWheelRadius, tankTuning, true);
		tank->addWheel(btVector3(-1, 0, 1), wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, false);
		tank->addWheel(btVector3(-1, 0, -1), wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, false);
        for (int i = 0; i < 4;i++) { 
            tank->getWheelInfo(i).m_suspensionStiffness = 0;
			tank->getWheelInfo(i).m_wheelsDampingRelaxation = 0;
			tank->getWheelInfo(i).m_wheelsDampingCompression = 0;
			tank->getWheelInfo(i).m_frictionSlip = 0;
			tank->getWheelInfo(i).m_rollInfluence = 0;
        }
    }

    glm::vec3 Tank::getPosition() {
        btTransform trans;
        tankChassis->getMotionState()->getWorldTransform(trans);
        btVector3 pos = trans.getOrigin();
        return glm::vec3(float(pos.getX()),float(pos.getY()),float(pos.getZ()));
    }

	void Tank::renderme() {
		btScalar m[16];
		int i;

		//btVector3 wheelColor(1, 0, 0);
		btVector3 worldBoundsMin, worldBoundsMax;

		for (int i = 0; i < 4; i++) {
			tank->updateWheelTransform(i, true);
			tank->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);
			//m_shapeDrawer->drawOpenGL(m, m_wheelShape, wheelColor, getDebugMode(), worldBoundsMin, worldBoundsMax);
		}
		//DemoApplication::renderme();
	}
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
		tank->applyEngineForce(tankEngineForce, 2);
		tank->setBrake(tankBreakingForce,2);
		tank->applyEngineForce(tankEngineForce, 3);
		tank->setBrake(tankBreakingForce, 3);
		tank->setSteeringValue(tankSteering, 0);
		tank->setSteeringValue(tankSteering, 1);
	}
	
}