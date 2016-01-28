#pragma once

#include <vector>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision\CollisionShapes\btBoxShape.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Wavefront.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "MeshTools.h" // Won't be needed in the final version
#include "Renderer.h"

namespace tankwars {
	class Tank {
	public:
		Tank(btDiscreteDynamicsWorld *dynamicsWorld,Renderer& renderer, btVector3 startingPosition);
		void addWheels();
		void update();
		void turn(bool left);
		void drive(bool forward);
		btRaycastVehicle* getAction();
		MeshInstance getTankMeshInstance(int i);
		glm::vec3 getPosition();
		btRigidBody* getRigidBody();
		void setHeadAndCanonRotation(btScalar angle);
		void setCanonRotation(btScalar angle);
	private:
		Renderer& renderer;
		btCollisionShape* tankBoxShape;
		void initializeTankMeshInstances(btVector3 startPos);
		//btVector3 startingPosition;
		btDiscreteDynamicsWorld* dnmcWorld;
		void setTankTuning();
		btTransform tr;
		btMotionState *tankMotionState;
		btRaycastVehicle::btVehicleTuning tankTuning;
		btRigidBody* tankChassis;
		btRaycastVehicle* tank;
		//btCollisionShape* frontWheel;
		//btCollisionShape* backWheel;

		//MESHES AND MESHINSTANCES
		btScalar headAndCanonRotationAngle;
		btScalar canonRotationAngle;

		glm::mat4x4 tankModelMat;
		Material tankMaterial;
		WavefrontModel tankBodyModel;
		WavefrontModel tankHeadModel;
		WavefrontModel tankCanonModel;
		Mesh* tankBodyMesh;
		Mesh* tankHeadMesh;
		Mesh* tankCanonMesh;
		// 0 = tankBody / 1 = tankHead / 2 = turret / 3-6 = wheels
		std::vector<MeshInstance> tankMeshInstances;					// how to add this shit to the renderer?
		//END MESHES AND MESHINSTANCES

		//FORCES
		float maxEngineForce = 1000.f;
		float defaultBreakingForce = 100.f;
		float tankEngineForce = 10.f;
		float tankBreakingForce = 0.f;// defaultBreakingForce;

		float	steeringIncrement = 0.04f;
		float	steeringClamp = 0.3f;
		float	tankSteering = 10;

		btScalar mass = 200;
		btScalar frontWheelRadius = 0.838f;
		btScalar backWheelRadius = 1.f;
		btScalar suspensionRestLength = 20;
		btVector3 wheelDirection;
		btVector3 wheelAxle;
		//END FORCES
	};

	
}