#include "Tank.h"
namespace tankwars {
	Tank::Tank(btRigidBody chassis) :tankTuning(), vehicleRaycaster(NULL), tankBoxShape(new btBoxShape(btVector3(2,1,1))), btRaycastVehicle( tankTuning, &tankChassis,vehicleRaycaster ),tankChassis(800,nullptr,tankBoxShape) {
		setTankTuning();
	}
	void Tank::setTankTuning() {
		tankTuning.m_frictionSlip = 0;
		tankTuning.m_maxSuspensionForce = 0;
		tankTuning.m_maxSuspensionTravelCm = 0;
		tankTuning.m_suspensionCompression = 0;
		tankTuning.m_suspensionDamping = 0;
		tankTuning.m_suspensionStiffness = 0;
	}
	void Tank::addToWorld(btDynamicsWorld &dynamicsWorld) {
		dynamicsWorld.addAction(this);
	}
	
}