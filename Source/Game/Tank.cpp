#include "Tank.h"
namespace tankwars {

	Tank::Tank( btDiscreteDynamicsWorld *dynamicsWorld, btVector3 startingPosition) :boxMesh(createBoxMesh(2, 1, 1)), dnmcWorld(dynamicsWorld), startingPosition(startingPosition), tr(btQuaternion(0, 0, 0, 1), startingPosition), tankMotionState(new btDefaultMotionState(tr)), tankTuning(), vehicleRaycaster(new btDefaultVehicleRaycaster(dynamicsWorld)), tankBoxShape(new btBoxShape(btVector3(2, 1, 1))), btRaycastVehicle(tankTuning, &tankChassis, vehicleRaycaster), tankChassis(mass, tankMotionState, tankBoxShape) {
        setTankTuning();
        tankChassis.setActivationState(DISABLE_DEACTIVATION);
        tankBoxShape->calculateLocalInertia(mass, btVector3(0, 0, 0));
		mat.diffuseColor = { 1,0,0 };
		initializeTankMeshInstances();

		addRigidBodiesToWorld();
		addToWorld();
		reset();

        //tr.setIdentity();
    }
	void Tank::addRigidBodiesToWorld() {
		dnmcWorld->addRigidBody(&tankChassis);
		
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
    void Tank::addToWorld() {
        dnmcWorld->addAction(this);
    }
    void Tank::addWheels() {
        addWheel(btVector3(1, 0, -1), wheelDirection, wheelAxle,suspensionRestLength,frontWheelRadius, tankTuning, true);
        addWheel(btVector3(1, 0, 1), wheelDirection, wheelAxle, suspensionRestLength, frontWheelRadius, tankTuning, true);
        addWheel(btVector3(-1, 0, 1), wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, false);
        addWheel(btVector3(-1, 0, -1), wheelDirection, wheelAxle, suspensionRestLength, backWheelRadius, tankTuning, false);
        for (int i = 0; i < 4;i++) { 
            m_wheelInfo[i].m_suspensionStiffness = 0;
            m_wheelInfo[i].m_wheelsDampingRelaxation = 0;
            m_wheelInfo[i].m_wheelsDampingCompression = 0;
            m_wheelInfo[i].m_frictionSlip = 0;
            m_wheelInfo[i].m_rollInfluence = 0;
        }

    }
    glm::vec3 Tank::getPosition() {
        btTransform trans;
        tankChassis.getMotionState()->getWorldTransform(trans);
        btVector3 pos = trans.getOrigin();
        return glm::vec3(float(pos.getX()),float(pos.getY()),float(pos.getZ()));
    }
    void Tank::reset() {
        tankChassis.setCenterOfMassTransform(btTransform::getIdentity());
        tankChassis.setAngularVelocity(btVector3(0, 0, 0));
        tankChassis.setLinearVelocity(btVector3(0, 0, 0));
        /*for (size_t i = 0; i < 4; i++) {
        m_wheelInfo[i].updateWheelTransform(i, true);
        }*/
    }
	void Tank::render(btScalar step) {
		update(step);
		btVector3 worldBoundsMin, worldBoundsMax;
		dnmcWorld->getBroadphase()->getBroadphaseAabb(worldBoundsMin, worldBoundsMax);
		ATTRIBUTE_ALIGNED16(btScalar) m[16];//107   279
		for (int i = 0; i < 4; i++) {
			m_wheelInfo[i].m_worldTransform.getOpenGLMatrix(m);
		}
		//fucking demo just calls non-existent func renderme() for the body rendering
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
	void Tank::update(btScalar step) {
		m_wheelInfo[0].m_steering = tankSteering;
		m_wheelInfo[1].m_steering = tankSteering;

		m_wheelInfo[2].m_engineForce = tankEngineForce;
		m_wheelInfo[2].m_brake = tankBreakingForce;
		m_wheelInfo[3].m_engineForce = tankEngineForce;
		m_wheelInfo[3].m_brake = tankBreakingForce;
		updateAction(dnmcWorld, step);
	}
}