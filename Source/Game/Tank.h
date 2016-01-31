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
		void driveBack(bool backward);
		btRaycastVehicle* getAction();
		MeshInstance getTankMeshInstance(int i);
		glm::vec3 getPosition();
		btRigidBody* getRigidBody();

		//Controller Functions
		void breakController();
		void driveController(float val);
		void turnController(float val);
		void turnTurretController(float val);
		void turnHeadAndTurretController(float val);
		void shoot();
		void adjustPower(bool increase);
	private:
		Renderer& renderer;
		btCollisionShape* tankBoxShape;
		void initializeTankMeshInstances(btVector3 startPos);
		//btVector3 startingPosition;
		btDiscreteDynamicsWorld* dnmcWorld;
		void setTankTuning();
		btTransform tr;
		btMotionState *tankMotionState;
		btDefaultVehicleRaycaster* tankVehicleRaycaster;
		btRaycastVehicle::btVehicleTuning tankTuning;
		btRigidBody* tankChassis;
		btRaycastVehicle* tank;

		
		//btCollisionShape* frontWheel;
		//btCollisionShape* backWheel;

		//Tank Meshes and MeshInstances

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
		//End Tank Meshes and MeshInstances

		//Tank Physics Variables

		float maxEngineForce = 1000.f;
		float defaultBreakingForce = 10.f;
		float maxBreakingForce = 100.f;					// what is this for?0.o
		float tankEngineForce = 0.f;
		float tankBreakingForce = 0.f;// defaultBreakingForce;

		float	steeringIncrement = 0.04f;
		float	steeringClamp = 0.2f;
		float	tankSteering = 0.f;

		btScalar mass = 800;
		btScalar wheelWidth = 0.4f;
		btScalar frontWheelRadius = 0.5f;//0.838f;
		btScalar backWheelRadius = 0.5f;//1.f;
		btScalar suspensionRestLength = 0.6;
		btVector3 wheelDirection;
		btVector3 wheelAxle;

		//End Tank Physics Variables

		//Tank Movement Variables
		btScalar shootingPower = 20;				//not adjusted
		btScalar shootingPowerIncrease = 10;		//not adjusted
		btScalar shootingPowerMax = 100;			//not adjusted

		btScalar turretMinAngle = -0.06;
		btScalar turretMaxAngle = 0.5f;				//not adjusted
		btScalar turretRotationAlpha = 0.01;		//not adjusted
		btScalar turretAngle = 0;
		btScalar headAndTurretRotationAlpha = 0.01;	//not adjusted
		btScalar headAndTurretAngle = 0;

		//End Tank Movement Variables
		class BulletHandler {
		public:
			BulletHandler(btDynamicsWorld* dnmcWorld, Renderer& renderer);
			void createNewBullet(btTransform& tr, btScalar headAngle, btScalar turretAngle, btScalar power);
			void updateBullets();
		private:
			btDynamicsWorld* dnmcWorld;
			Renderer& renderer;
			btSphereShape bulletShape;
			Mesh bulletMesh;
			size_t bulletMax = 5000;
			size_t bulletCounter = 0;
			btRigidBody* bulletRigidBodies[5000];
			MeshInstance* bulletInstances[5000];
			//std::vector<btRigidBody> bulletRigidBodies;
			//std::vector<MeshInstance> bulletInstances;
			Material bulletMat;
		};
		BulletHandler bulletHandler;
	};

	
}