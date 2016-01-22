#include "Tank.h"
namespace tankwars {
	Tank::Tank(btDiscreteDynamicsWorld *dynamicsWorld,btVector3 startingPosition) :dnmcWorld(dynamicsWorld),startingPosition(startingPosition), tr(btQuaternion(0, 0, 0, 1), startingPosition), tankMotionState(new btDefaultMotionState(tr)),tankTuning(), vehicleRaycaster(new btDefaultVehicleRaycaster(dynamicsWorld)), tankBoxShape(new btBoxShape(btVector3(2,1,1))), btRaycastVehicle( tankTuning, &tankChassis,vehicleRaycaster ),tankChassis(mass,tankMotionState,tankBoxShape) {
		setTankTuning();
		tankChassis.setActivationState(DISABLE_DEACTIVATION);
		tankBoxShape->calculateLocalInertia(mass, btVector3(0, 0, 0));
		tr.setIdentity();
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
	Tank::~Tank() {

	}
	
}