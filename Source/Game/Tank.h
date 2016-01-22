#pragma once
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <BulletCollision\CollisionShapes\btBoxShape.h>
#include <glm/glm.hpp>

namespace tankwars {
	class Tank : public btRaycastVehicle {
	public:
		Tank(btDiscreteDynamicsWorld *dynamicsWorld, btVector3 startingPosition);
		void addToWorld();
		void addWheels();
		void reset();
		glm::vec3 getPosition();
		~Tank();
	private:
		btVector3 startingPosition;
		btDiscreteDynamicsWorld* dnmcWorld;
		void setTankTuning();
		btMotionState *tankMotionState;
		btTransform tr;
		btVehicleTuning tankTuning;
		btRigidBody tankChassis;
		btVehicleRaycaster* vehicleRaycaster;
		btCollisionShape* tankBoxShape;
		//btCollisionShape* frontWheel;
		//btCollisionShape* backWheel;
	};
	static btScalar mass = 800;
	static btScalar frontWheelRadius = 0.838f;
	static btScalar backWheelRadius = 1.f;
	static btScalar suspensionRestLength = 20;
	static btVector3 wheelDirection(0, -1, 0);
	static btVector3 wheelAxle(-1, 0, 0);
	
}