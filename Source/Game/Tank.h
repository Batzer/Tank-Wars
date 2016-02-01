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
		struct Bullet {
			bool active;
			bool disableMe;
			btRigidBody* bulletBody;
			MeshInstance bulletMeshInstance;
			Bullet( btRigidBody* body, MeshInstance inst) :active(true),disableMe(false), bulletBody(body), bulletMeshInstance(inst) {};
		};
		Tank(btDiscreteDynamicsWorld *dynamicsWorld, Renderer& renderer, btVector3 startingPosition);
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
        btCompoundShape* compoundShape;
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

		//Tank Meshes and MeshInstances

		glm::mat4x4 tankModelMat;
		Material tankMaterial;
		WavefrontModel tankBodyModel;
		WavefrontModel tankHeadModel;
		WavefrontModel tankCanonModel;
		WavefrontModel tankLeftFrontWheelModel;
		WavefrontModel tankRightFrontWheelModel;
		WavefrontModel tankLeftBackWheelModel;
		WavefrontModel tankRightBackWheelModel;
		Mesh* tankBodyMesh;
		Mesh* tankHeadMesh;
		Mesh* tankCanonMesh;
		Mesh* tankLeftFrontWheelMesh;
		Mesh* tankRightFrontWheelMesh;
		Mesh* tankLeftBackWheelMesh;
		Mesh* tankRightBackWheelMesh;

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

		btScalar mass = 80;
		btScalar wheelWidth = 0.4f;
		btScalar frontWheelRadius = .838f;//0.838f;
		btScalar backWheelRadius = .838f; //1.f;
		btScalar suspensionRestLength = 0.6f;
		btVector3 wheelDirection;
		btVector3 wheelAxle;

		//End Tank Physics Variables

		//Tank Movement Variables
		btScalar shootingPower = 20;				//not adjusted
		btScalar shootingPowerIncrease = 10;		//not adjusted
		btScalar shootingPowerMax = 100;			//not adjusted

		btScalar turretMinAngle = -.06f;
		btScalar turretMaxAngle = .5f;				//not adjusted
		btScalar turretRotationAlpha = .01f;		//not adjusted
		btScalar turretAngle = .0f;
		btScalar headAndTurretRotationAlpha = .01f;	//not adjusted
		btScalar headAndTurretAngle = .0f;

		//End Tank Movement Variables
		class BulletHandler {
		public:
			BulletHandler(btDynamicsWorld* dnmcWorld, Renderer& renderer);
			void createNewBullet(btTransform& tr, btScalar headAngle, btScalar turretAngle, btScalar power);
			void updateBullets();
			void removeBullet(int index);
		private:
			int tankID;
			btDynamicsWorld* dnmcWorld;
			Renderer& renderer;
			btSphereShape bulletShape;
			Mesh bulletMesh;
			size_t bulletMax = 5000;
			size_t bulletCounter = 0;
			Bullet* bullets[5000];					//replace with an std::vector
			Material bulletMat;
		};
		BulletHandler bulletHandler;
	};
}
