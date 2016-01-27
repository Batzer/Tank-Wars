#pragma once

#include <vector>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionShapes\btBoxShape.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "MeshInstance.h"
#include "MeshTools.h" // Won't be needed in the final version
#include "Renderer.h"

namespace tankwars {
	class Tank : public btRaycastVehicle {
	public:
		Tank(btDiscreteDynamicsWorld *dynamicsWorld, btVector3 startingPosition);
		void addToWorld();
		void addWheels();
		void reset();
		void render(btScalar step);

		void turn(bool left);
		void drive(bool forward);
		MeshInstance getTankMeshInstance(int i);
		glm::vec3 getPosition();
	private:
		void addRigidBodiesToWorld();
		void initializeTankMeshInstances();
		void update(btScalar step);
		btVector3 startingPosition;
		btDiscreteDynamicsWorld* dnmcWorld;
		void setTankTuning();
		btTransform tr;
		btMotionState *tankMotionState;
		btVehicleTuning tankTuning;
		btRigidBody tankChassis;
		//btVehicleRaycaster* vehicleRaycaster;
		btCollisionShape* tankBoxShape;
		//btCollisionShape* frontWheel;
		//btCollisionShape* backWheel;

		//MESHES AND MESHINSTANCES
		Mesh boxMesh;
		Material mat;
		//use objloader to load the meshes
		// 0 = tankBody / 1 = tankHead / 2 = turret / 3-6 = wheels
		std::vector<MeshInstance> TankMeshInstances;					// how to add this shit to the renderer?
		//END MESHES AND MESHINSTANCES
		//FORCES
		float maxEngineForce = 1000.f;
		float defaultBreakingForce = 100.f;
		float tankEngineForce = 0.f;
		float tankBreakingForce = defaultBreakingForce;

		float	steeringIncrement = 0.04f;
		float	steeringClamp = 0.3f;
		float	tankSteering = 0;
		//END FORCES
	};

	static btScalar mass = 200;
	static btScalar frontWheelRadius = 0.838f;
	static btScalar backWheelRadius = 1.f;
	static btScalar suspensionRestLength = 20;
	static btVector3 wheelDirection(0, -1, 0);
	static btVector3 wheelAxle(-1, 0, 0);
	
}