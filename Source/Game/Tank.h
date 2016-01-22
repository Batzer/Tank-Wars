#pragma once
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionShapes\btBoxShape.h>

namespace tankwars {
	class Tank : public btRaycastVehicle {
	public:
		Tank(btRigidBody chassis);
		void addToWorld(btDynamicsWorld &dynamicsWorld);
	private:
		void setTankTuning();
		btTransform tr;
		btVehicleTuning tankTuning;
		btRigidBody tankChassis;
		btVehicleRaycaster* vehicleRaycaster;
		btCollisionShape* tankBoxShape;
		btCollisionShape* frontWheel;
		btCollisionShape* backWheel;
		//int wheelInstances[4];
		//btWheelInfo wheels[4];
	};
}